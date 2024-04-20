#include "rect.h"
#include "input.h"
#include "audio.h"

#include <cstdint>
#include <cassert>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>

bool running = false;
bool paused = false;

namespace platform 
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	Mix_Chunk* fireWaveSound;
	Mix_Chunk* invaderKilledSound;

	std::array<Mix_Chunk*, SOUND_COUNT> mixChunks;

	uint64_t getTicks()
	{
		return SDL_GetTicks64();
	}

	void createWindow(int width, int height)
	{
		assert(SDL_Init(SDL_INIT_AUDIO) >= 0 && "Unable to init SDL");
		assert(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) >= 0 && "Unable to open audio device");

		window = SDL_CreateWindow("Invaders SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
		assert(window != nullptr && "Unable to create window");

		renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);
		assert(renderer != nullptr && "Unable to create renderer");

		assert(IMG_Init(IMG_INIT_PNG) != 0 && "Unable to init SDL_Image");
		SDL_Surface* surface = IMG_Load("asset/sprites.png");
		assert(surface != nullptr && "Unable to load sprites.png");

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		assert(texture != nullptr && "Unable to load sprites.png");
		SDL_FreeSurface(surface);

		running = true;
		paused = false;
	}

	void updateWindow()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT: keyDown(KEY_CODE_LEFT); break;
				case SDLK_RIGHT: keyDown(KEY_CODE_RIGHT); break;
				case SDLK_SPACE: keyDown(KEY_CODE_SPACE); break;
				case SDLK_ESCAPE: keyDown(KEY_CODE_ESCAPE); break;
				}
			}
			else if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT: keyUp(KEY_CODE_LEFT); break;
				case SDLK_RIGHT: keyUp(KEY_CODE_RIGHT); break;
				case SDLK_SPACE: keyUp(KEY_CODE_SPACE); break;
				case SDLK_ESCAPE: keyUp(KEY_CODE_ESCAPE); break;
				}
			}
		}		
	}

	void addBuffTexture(const Rect& srcRect, const Rect& dstRect)
	{
		SDL_Rect src{ srcRect.x, srcRect.y, srcRect.w, srcRect.h };
		SDL_FRect dst{ dstRect.x, dstRect.y, dstRect.w, dstRect.h };
		SDL_RenderCopyF(renderer, texture, &src, &dst);		
	}

	void addBuffRect(const Rect& rect, uint32_t rgb)
	{
		auto r = (rgb >> 24);
		auto g = (rgb >> 16) & 0xff;
		auto b = rgb & 0xff;
		SDL_SetRenderDrawColor(renderer, r, g, b, 1);

		SDL_Rect src{ rect.x, rect.y, rect.w, rect.h };
		SDL_RenderFillRect(renderer, &src);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	}

	void clearBuff() 
	{ 
		SDL_RenderClear(renderer);
	}

	void renderBuff() 
	{
		SDL_RenderPresent(renderer); 
	}
	
	void playSound(int soundId) 
	{ 
		auto path = getSoundPath(soundId);
		assert(path != "", "Sound not found");

		auto chunk = mixChunks[soundId];
		if (chunk == nullptr)
		{
			auto wave = Mix_LoadWAV(path.c_str());			
			assert(wave != nullptr, "Unable to load: " + path);
			mixChunks[soundId] = wave;

		}
		Mix_PlayChannel(0, chunk, 0); 
	}

	void exit()
	{
		Mix_FreeChunk(fireWaveSound);
		Mix_FreeChunk(invaderKilledSound);
		Mix_CloseAudio();
		Mix_Quit();
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};