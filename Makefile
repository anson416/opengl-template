COMPILER = g++
FLAGS = -std=c++17 -O1 -Wall -m64 \
	-I./dependencies/include \
	-I./utils \
	-L./dependencies/library \
	-framework OpenGL

all: main.cpp
	$(COMPILER) $(FLAGS) -o main \
		$(shell find . -type f -iregex ".*\.cpp") \
		./dependencies/library/libGLEW.2.2.0.dylib \
		./dependencies/library/libglfw.3.3.dylib
	./main
