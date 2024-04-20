#pragma once

#include <cstdint>
#include <SDL.h>

uint64_t platformGetTicks()
{
	return SDL_GetTicks64();
}
