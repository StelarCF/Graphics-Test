#pragma once

// Just for nano colours (am that awful)
#ifdef __linux__
#define __LINUX__
#endif

#if defined(_WIN32)
#include <SDL.h>
#elif defined(__LINUX__)
#include <SDL2/SDL.h>
#elif defined(__APPLE__)
#error We're apples forever, apples together / We're family, but so much more / No matter what comes, we will face the weather / We're apples to the core
#endif

void ProcEvent(SDL_Event ev);
void renderInit();
void render();
void renderFin();
