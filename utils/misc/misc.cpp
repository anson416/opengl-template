#include "misc.h"

#include "GL/glew.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

void showOpenGLInfo(void)
{
    const GLubyte* name = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* glversion = glGetString(GL_VERSION);
    
    std::cout << "----- OpenGL Info -----" << std::endl;
    std::cout << "OpenGL company: " << name << std::endl;
    std::cout << "Renderer name: " << renderer << std::endl;
    std::cout << "OpenGL version: " << glversion << std::endl;
    std::cout << "-----------------------" << std::endl;
}

int randint(int a, int b)
{
    return std::rand() % (b - a + 1) + a;
}

/* Load OBJ file (cannot load all OBJ files) */
Model loadOBJ(const char* objPath)
{
	struct V {
		/* Struct for identify if a vertex has showed up */
		unsigned int index_position, index_uv, index_normal;
		bool operator == (const V& v) const {
			return index_position == v.index_position && index_uv == v.index_uv && index_normal == v.index_normal;
		}
		bool operator < (const V& v) const {
			return (index_position < v.index_position) ||
				(index_position == v.index_position && index_uv < v.index_uv) ||
				(index_position == v.index_position && index_uv == v.index_uv && index_normal < v.index_normal);
		}
	};

	std::vector<glm::vec3> temp_positions;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	std::map<V, unsigned int> temp_vertices;

	Model model;
	unsigned int num_vertices = 0;

	std::cout << "INF: Loading OBJ " << objPath << "..." << std::endl;

	std::ifstream file(objPath);

	/* Check for error */
	if (file.fail()) {
		std::cerr << "ERR: Failed to load " << objPath << std::endl;
		exit(1);
	}
    
    for (std::string line; std::getline(file, line); ) {
        std::istringstream in(line);
        std::vector<std::string> line_vec=std::vector<std::string>(std::istream_iterator<std::string>(in), std::istream_iterator<std::string>());
        
        if(line_vec.size() == 0)
            continue;

		/* Process the OBJ file */
		const char *lineHeader=line_vec[0].c_str();

		if (strcmp(lineHeader, "v") == 0) {  /* Geometric vertices */
            // std::assert(line_vec.size() == 4);
			glm::vec3 position = glm::vec3(std::atof(line_vec[1].c_str()), std::atof(line_vec[2].c_str()), std::atof(line_vec[3].c_str()));
			temp_positions.push_back(position);
		}
		else if (strcmp(lineHeader, "vt") == 0) {  /* Texture coordinates */
            // std::assert(line_vec.size() == 3);
			glm::vec2 uv = glm::vec2(std::atof(line_vec[1].c_str()), std::atof(line_vec[2].c_str()));
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {  /* Vertex normals */
            // std::assert(line_vec.size() == 4);
			glm::vec3 normal = glm::vec3(std::atof(line_vec[1].c_str()), std::atof(line_vec[2].c_str()), std::atof(line_vec[3].c_str()));
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {  /* Face elements */
            // std::assert((line_vec.size() == 4 || line_vec.size() == 5));
            int n = line_vec.size() - 1;
            if(n != 3 && n != 4) {
                std::cerr << "ERR: There may exist some errors while loading the OBJ." << std::endl;
                std::cerr << "     Error content: [" << line << "]" << std::endl;
                std::cerr << "     Can only handle triangles or quads in OBJ for now." << std::endl;
                exit(1);
            }

            std::vector<V> vertices(n);
			for (int i = 0; i < n; i++) {
                std::stringstream ss(line_vec[i+1]);
                std::string item;
                char delim='/';
                getline(ss, item, delim); int ip = std::atoi(item.c_str());
                getline(ss, item, delim); int it = std::atoi(item.c_str());
                getline(ss, item, delim); int in = std::atoi(item.c_str());
                vertices[i].index_position = ip;
                vertices[i].index_uv = it;
                vertices[i].index_normal = in;
			}
            
            std::vector<int> idxs;
			for (int i = 0; i < n; i++) {
				if (temp_vertices.find(vertices[i]) == temp_vertices.end()) {  /* The vertex is new */
					Vertex vertex;
					vertex.pos = temp_positions[vertices[i].index_position - 1];
					vertex.uv = temp_uvs[vertices[i].index_uv - 1];
					vertex.normal = temp_normals[vertices[i].index_normal - 1];

					model.vertices.push_back(vertex);
                    idxs.push_back(num_vertices);
					temp_vertices[vertices[i]] = num_vertices;
					num_vertices += 1;
				}
				else {  /* Reuse existing vertex */
					unsigned int index = temp_vertices[vertices[i]];
                    idxs.push_back(index);
				}
			}
            if(n == 3) {
                model.indices.push_back(idxs[0]);
                model.indices.push_back(idxs[1]);
                model.indices.push_back(idxs[2]);
            }
            else {  /* Split a quad into two triangles */
                model.indices.push_back(idxs[0]);
                model.indices.push_back(idxs[1]);
                model.indices.push_back(idxs[2]);

                model.indices.push_back(idxs[0]);
                model.indices.push_back(idxs[2]);
                model.indices.push_back(idxs[3]);
            }
		}
		else {  /* Skip it. It is not a vertex, texture coordinate, normal nor face. */
            // std::cout << "INF: Skipped: [" << line << "]" << std::endl;
		}
	}
    /* NOTE: vertices with the same position but different uv or normal are counted as different vertices during OBJ loading */
	// std::cout << "INF: There are " << num_vertices << " vertices and " << model.indices.size() / 3 << " triangles in the OBJ.\n" << std::endl;
    
	return model;
}

void calBboxAndCenter(const std::vector<Vertex>& verts)
{
    float INF=1e+6;
    glm::vec3 p1(INF, INF, INF);
    glm::vec3 p2=-p1;

    for(int i=0; i<verts.size(); i++) {
        p1.x = glm::min(p1.x, verts[i].pos.x);
        p1.y = glm::min(p1.y, verts[i].pos.y);
        p1.z = glm::min(p1.z, verts[i].pos.z);

        p2.x = glm::max(p2.x, verts[i].pos.x);
        p2.y = glm::max(p2.y, verts[i].pos.y);
        p2.z = glm::max(p2.z, verts[i].pos.z);
    }
    
    glm::vec3 center = 0.5f*(p1+p2);
    glm::vec3 bbox = p2-p1;
    printf("Center %f %f %f\n", center.x, center.y, center.z);
    printf("DX %f DY %f DZ %f\n", bbox.x, bbox.y, bbox.z);
}

void normalizeToUnitBbox(std::vector<Vertex>& verts)
{
    float INF=1e+6;
    glm::vec3 p1(INF, INF, INF);
    glm::vec3 p2=-p1;

    for(int i=0; i<verts.size(); i++) {
        p1.x = glm::min(p1.x, verts[i].pos.x);
        p1.y = glm::min(p1.y, verts[i].pos.y);
        p1.z = glm::min(p1.z, verts[i].pos.z);

        p2.x = glm::max(p2.x, verts[i].pos.x);
        p2.y = glm::max(p2.y, verts[i].pos.y);
        p2.z = glm::max(p2.z, verts[i].pos.z);
    }
    
    glm::vec3 center = 0.5f*(p1+p2);
    glm::vec3 bbox = p2-p1;

    float S=glm::max(glm::max(bbox.x, bbox.y), bbox.z);
    for(int i=0; i<verts.size(); i++)
        verts[i].pos = (verts[i].pos - center)/S; 
}
