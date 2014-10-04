all:
	g++ lodepng.cpp lodepng.h vmath.h Render.cpp Render.h main.cpp -lSDL2 -lGLEW -lGLU -lGL
