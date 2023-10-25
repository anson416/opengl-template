#pragma once

#include "glm/glm.hpp"

#include <vector>

/* Rescale RGB values from [0, 255] to [0, 1] */
#define NORM_RGB(x) ((x) / 255.0f)
#define R(r) NORM_RGB(r)
#define G(g) NORM_RGB(g)
#define B(b) NORM_RGB(b)

/* Compute maximum between a and b */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* Compute minimum between a and b */
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* Clamp number num to range [a, b] */
#define CLAMP(num, a, b) (MAX((a), MIN((num), (b))))

struct Vertex {
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;
};

struct Model {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

void showOpenGLInfo(void);
int randint(int a, int b);
double randreal(double a, double b);
Model loadOBJ(const char* objPath);
void calBboxAndCenter(const std::vector<Vertex>& verts);
void normalizeToUnitBbox(std::vector<Vertex>& verts);
