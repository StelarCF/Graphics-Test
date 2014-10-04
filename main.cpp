/*#include "VGraphics.h"
#include "Render.h"
#include <memory.h>

//int CALLBACK WinMain(_In_  HINSTANCE hInstance,	_In_  HINSTANCE hPrevInstance,	_In_  LPSTR lpCmdLine,	_In_  int nCmdShow) {
int main() {
	VG::Window wnd(800, 600, 24, "OpenGL Test", false);
	wnd.SetCurrent();
	glewInit();
	/*float *data = new float[800 * 600 * 4];
	memset(data, 0, 800 * 600 * 4 * sizeof(float));
	glBindTexture(GL_TEXTURE_2D, directRender.getId());
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, edgeDetect.getId());
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, finalScene.getId());
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);//I'll implement multipass rendering after I get a simple scene up
#ifndef _DEBUG
#ifdef _WIN32
	FreeConsole();
#endif
#endif
	renderInit(wnd);
	while (wnd.isOpen()) {
		VG::VGEvent ev;
		wnd.processEvents();
		while (wnd.getNextEvent(ev)) { //WM_SYSCOMMAND
			if (ev.type == VG::Closed) {
				break;
			}
		}
		render(wnd);
		wnd.swapBuffers();
	}
	renderFin(wnd);
	return 0;
}*/

/// IMPORTANT NOTICE: You need to set debug exe to always run with the Nvidia graphics processor or it crash :(

#if defined(_WIN32)
#include <SDL.h>
#elif defined(__linux__)
#include <SDL2/SDL.h>
#elif defined(__APPLE__)
#error Apples in Bloom
#endif
#include <GL/glew.h>
#include <stdio.h>
#include "Render.h"
#undef main // SDL likes to redeclare your main as SDL_main... for no reason

SDL_Surface *screen;
SDL_GLContext context;

int main() {
	printf ("Initializing SDL 2.0...\n");
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	//initialize defaults, video and audio
	if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) == -1) {
		printf("ERROR: Could not initialize SDL: %s. \n", SDL_GetError());
		exit(-1);
	}
	printf("SDL successfully initialized.\nCreating window...");
	SDL_Window *window;
	window = SDL_CreateWindow("SDL Window", 50, 50, 1920, 1080, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
	glewInit();
	printf ("Window created.\nChecking OGL Context:\n");
	printf ("Vendor: %s\nRenderer: %s\nVersion: %s\nShading Language Version: %s\nExtensions: %s\n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), glGetString(GL_SHADING_LANGUAGE_VERSION), glGetString(GL_EXTENSIONS));
	SDL_GL_MakeCurrent(window, context);
	renderInit();
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				return 0;
				break;
			default:
				ProcEvent(ev);
			}
		}
		//GL rendering
		render();
		SDL_GL_SwapWindow(window);
	}
	renderFin();
	printf("Quitting SDL.\n");
	SDL_Quit();
	printf("Quitting....");
	exit(0);
	return 0;
}
