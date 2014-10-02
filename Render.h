#pragma once
#include "VGraphics.h"
#include <SDL.h>

void ProcEvent(SDL_Event ev);
void renderInit();
void render();
void renderFin();