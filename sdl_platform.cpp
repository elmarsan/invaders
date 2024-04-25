#include "util.h"
#include "input.h"
#include "audio.h"
#include "asset.h"

#include <cstdint>
#include <cassert>
#include <vector>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

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

	void updateWindow()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				platform::exit();
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

	Rect getWindowBarDims()
	{
		int top;
		int left;
		int bottom;
		int right;
		SDL_GetWindowBordersSize(window, &top, &left, &bottom, &right);
		return Rect{ static_cast<float>(top), static_cast<float>(left), static_cast<float>(bottom), static_cast<float>(right) };
	}

	void addBuffTexture(const Rect& srcRect, const Rect& dstRect)
	{
		SDL_Rect src{ srcRect.x, srcRect.y, srcRect.w, srcRect.h };
		SDL_FRect dst{ dstRect.x, dstRect.y, dstRect.w, dstRect.h };
		SDL_RenderCopyF(renderer, texture, &src, &dst);
	}

	void addBuffFont(const Point2D& dstPoint, const std::string text)
	{
		std::vector<Point2D> fontPoints;

		for (const auto& c : text)
		{
			switch (c)
			{
			case 'A': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_A]); break;
			case 'B': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_B]); break;
			case 'C': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_C]); break;
			case 'D': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_D]); break;
			case 'E': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_E]); break;
			case 'F': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_F]); break;
			case 'G': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_G]); break;
			case 'H': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_H]); break;
			case 'I': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_I]); break;
			case 'J': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_J]); break;
			case 'K': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_K]); break;
			case 'L': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_L]); break;
			case 'M': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_M]); break;
			case 'N': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_N]); break;
			case 'O': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_O]); break;
			case 'P': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_P]); break;
			case 'Q': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_Q]); break;
			case 'R': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_R]); break;
			case 'S': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_S]); break;
			case 'T': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_T]); break;
			case 'U': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_U]); break;
			case 'V': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_V]); break;
			case 'W': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_W]); break;
			case 'X': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_X]); break;
			case 'Y': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_Y]); break;
			case 'Z': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_Z]); break;
			case '0': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_0]); break;
			case '1': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_1]); break;
			case '2': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_2]); break;
			case '3': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_3]); break;
			case '4': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_4]); break;
			case '5': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_5]); break;
			case '6': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_6]); break;
			case '7': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_7]); break;
			case '8': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_8]); break;
			case '9': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_9]); break;
			case '=': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_EQUAL]); break;
			case '*': fontPoints.emplace_back(spriteFonts[FONT_CHARACTER_ASTERISK]); break;
				// For blank spaces, a point outside the limits of the texture atlas is used.
			case ' ': fontPoints.emplace_back(Point2D{ -100, -100 }); break;
			}
		}

		int w = spriteFontWidth * spriteScaleX;
		int h = spriteFontHeight * spriteScaleY;

		SDL_FRect dst{ dstPoint.x, dstPoint.y, w, h };
		for (const auto& p : fontPoints)
		{
			SDL_Rect src{ p.x, p.y, spriteFontWidth, spriteFontHeight };
			SDL_RenderCopyF(renderer, texture, &src, &dst);
			dst.x += w;
		}
	}

	void addBuffRect(const Rect& rect, uint32_t rgb, bool filled)
	{
		auto r = (rgb >> 24);
		auto g = (rgb >> 16) & 0xff;
		auto b = rgb & 0xff;
		SDL_SetRenderDrawColor(renderer, r, g, b, 1);

		SDL_Rect src{ rect.x, rect.y, rect.w, rect.h };

		if (filled)
		{
			SDL_RenderFillRect(renderer, &src);
		}
		else
		{
			SDL_RenderDrawRect(renderer, &src);
		}		
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
		if (Mix_Playing(0) != 1)
		{
			Mix_PlayChannel(0, chunk, 0);
		}
	}
};