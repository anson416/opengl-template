COMPILER = g++
FLAGS = -std=c++17 -O1 -Wall -m64 \
	-I./dependencies/include \
	-I./utils \
	-L./dependencies/library \
	-framework OpenGL
EXECUTABLE = main

all: main.cpp
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) \
		$(shell find . -type f -iregex ".*\.cpp") \
		./dependencies/library/libGLEW.2.2.0.dylib \
		./dependencies/library/libglfw.3.3.dylib
	./main

clean:
	rm -rf $(EXECUTABLE)
