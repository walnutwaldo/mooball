all: graphics.cpp graphics.h main.cpp moosball.cpp moosball.h $(wildcard bots/*.h) $(wildcard bots/*.cpp)
	g++ main.cpp graphics.cpp moosball.cpp $(wildcard bots/*.cpp) -std=c++11 -I/usr/include/GL -lglut -lGL -lGLU -Ibots -I.
