#include "entities.h"

#include <iostream>
#include <array>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <cassert>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* texture;

Mix_Chunk* fireWaveSound;
Mix_Chunk* invaderKilledSound;

static bool running = true;
static bool paused = false;
static bool debug = true;
static int score = 0;
static int lifes = 3;

static int screenWidth = 800;
static int screenHeight = 640;

const int enemySpriteWidth = 16;

Projectile playerProjectile{};

Player player{};

std::array<Enemy, 55> enemies{};
std::array<Projectile, 3> enemyProjectiles{};

std::array<Obstacle, 4> obstacles{};

[[nodiscard]] static SDL_Rect SDLRect(const Rect& rect)
{
	return SDL_Rect{ (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h };
}

[[nodiscard]] static SDL_FRect SDLFRect(const Rect& rect)
{
	return SDL_FRect{ rect.x, rect.y, rect.w, rect.h };
}

void initSDL()
{
	assert(SDL_Init(SDL_INIT_AUDIO) >= 0 && "Unable to init SDL");
	assert(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) >= 0 && "Unable to open audio device");

	fireWaveSound = Mix_LoadWAV("asset/shoot.wav");
	invaderKilledSound = Mix_LoadWAV("asset/invaderkilled.wav");
	assert(fireWaveSound != nullptr && "Unable to load shoot.wav");
	assert(invaderKilledSound != nullptr && "Unable to load invaderkilled.wav");

	window = SDL_CreateWindow("Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
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

// TODO: Refactor using only one for loop
void initGame()
{
	float enemySizeX = enemySpriteWidth * spriteScaleX;

	float lastY = 0;
	float yOffset = 50;

	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = 50;
		enemies[i] = Enemy{ SPRITE_ENEMY_0, x, y };
	}
	lastY += yOffset;

	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 11] = Enemy{ SPRITE_ENEMY_1, x, y };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 22] = Enemy{ SPRITE_ENEMY_1, x, y };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 33] = Enemy{ SPRITE_ENEMY_2, x, y };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 44] = Enemy{ SPRITE_ENEMY_2, x, y };
	}

	float obstacleXOffset = 0;

	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i] = Obstacle{ i * 200 + obstacleXOffset, (float)screenHeight - 150 };
	}
}

void checkCollisions()
{	
	// Check if some enemy collides with player or player projectile
	if (playerProjectile.IsAlive())
	{
		for (auto& e : enemies)
		{
			if (!e.IsAlive())
			{
				continue;
			}

			const auto eCollider = e.GetCollider();
			const auto projCollider = playerProjectile.GetCollider();

			if (projCollider.intersects(eCollider))
			{
				Mix_PlayChannel(0, invaderKilledSound, 0);
				std::cout << "** Enemy destroyed **" << std::endl;
				// e.Destroy();
				e.Die();
				playerProjectile.Destroy();
				score += e.GetScore();
				break;
			}
			else if (player.GetCollider().intersects(eCollider))
			{
				std::cout << "YOU DIE" << std::endl;
				running = false;
				break;
			}
		}
	}
	
	// Check if some enemy projectile collides with player or obstacles
	for (auto& proj : enemyProjectiles)
	{
		if (proj.IsAlive())
		{
			continue;
		}

		// Check if projectile has reached player
		if (proj.GetCollider().intersects(player.GetCollider()))
		{
			if (--lifes == 0)
			{
				std::cout << "YOU DIE" << std::endl;
				running = false;
			}
			proj.Destroy();
			break;
		}

		// Check if projectile collides with any obstacle
		for (auto& o : obstacles)
		{
			if (!o.IsAlive())
			{
				continue;
			}
			if (proj.GetCollider().intersects(o.GetCollider()))
			{
				o.ReceiveProjectile();
				proj.Destroy();
				break;
			}
		}
	}
}

void renderScore()
{
	auto thousands = score / 1000;
	auto hundreds = (score % 1000) / 100;
	auto tens = (score % 100) / 10;
	auto units = (score % 10);

	auto digitTextures = {
		SDLRect(spriteNumbers[thousands]),
		SDLRect(spriteNumbers[hundreds]),
		SDLRect(spriteNumbers[tens]),
		SDLRect(spriteNumbers[units]),
	};

	int w = spriteFontW * spriteFontScaleX;
	int h = spriteFontY * spriteFontScaleY;
	SDL_FRect dst{ 0, 0, w, h };
	for (const auto& dt : digitTextures)
	{
		SDL_RenderCopyF(renderer, texture, &dt, &dst);
		dst.x += w;
	}
}

void renderLifes()
{
	auto numSrc = SDLRect(spriteNumbers[lifes]);
	SDL_FRect numDst{ 20, screenHeight - 40, spriteFontW * spriteFontScaleX, spriteFontY * spriteFontScaleY };
	SDL_RenderCopyF(renderer, texture, &numSrc, &numDst);

	SDL_FRect dst{ 60, screenHeight - 40, player.GetW(), player.GetH() };
	for (int i = 0; i < lifes; i++)
	{
		SDL_Rect src = SDLRect(player.GetTexture());
		SDL_RenderCopyF(renderer, texture, &src, &dst);
		dst.x += player.GetW();
	}
}

void renderObstacles()
{
	for (const auto& o : obstacles)
	{
		if (o.IsAlive())
		{
			SDL_Rect src = SDLRect(o.GetTexture());
			SDL_FRect dst = SDL_FRect{ o.GetPosX(), o.GetPosY(), o.GetW(), o.GetH() };
			SDL_RenderCopyF(renderer, texture, &src, &dst);
		}
	}
}

void renderProjectiles()
{
	// Render player projectile
	if (playerProjectile.IsAlive())
	{
		const auto& frame = playerProjectile.GetTexture();
		SDL_Rect rect{ frame.x, frame.y, frame.w, frame.h };
		SDL_FRect dst{ playerProjectile.GetPosX(), playerProjectile.GetPosY(), playerProjectile.GetW(), playerProjectile.GetH() };
		SDL_RenderCopyF(renderer, texture, &rect, &dst);
	}

	// Render enemy projectiles
	for (const auto& proj : enemyProjectiles)
	{
		if (proj.IsAlive())
		{
			const auto& frame = proj.GetTexture();
			SDL_Rect src = SDLRect(frame);
			SDL_FRect dst{ proj.GetPosX(), proj.GetPosY(), proj.GetW(), proj.GetH() };
			SDL_RenderCopyF(renderer, texture, &src, &dst);
		}
	}
}

void renderShips()
{
	// Render enemies
	for (const auto& e : enemies)
	{			
		if (e.IsAlive())
		{
			const auto& frame = e.GetTexture();		
			SDL_Rect rect{ frame.x, frame.y, frame.w, frame.h };
			SDL_FRect dst{ e.GetPosX(), e.GetPosY(), e.GetW(), e.GetH() };
			SDL_RenderCopyF(renderer, texture, &rect, &dst);
		}
	}

	// Render player
	const auto& frame = player.GetTexture();
	SDL_Rect rect{ frame.x, frame.y, frame.w, frame.h };
	SDL_FRect dst{ player.GetPosX(), player.GetPosY(), player.GetW(), player.GetH() };
	SDL_RenderCopyF(renderer, texture, &rect, &dst);
}

// An enemy can fire if it's in front of player.
std::vector<int> getCanShootEnemyIds()
{
	std::vector<int> canShootEnemyIds{};

	for (int i = 0; i < enemies.size(); i++)
	{
		if (!enemies[i].IsAlive())
		{
			continue;
		}
		if (i < 44)
		{
			bool nothingInFront = true;
			for (int j = i + 11; j < enemies.size(); j += 11)
			{
				if (enemies[j].IsAlive())
				{
					nothingInFront = false;
					break;
				}
			}

			if (nothingInFront)
			{
				canShootEnemyIds.push_back(i);
			}
		}
		// Enemy is alive and it's on the last row
		else
		{
			canShootEnemyIds.push_back(i);
		}
	}

	return canShootEnemyIds;
}

void updateEnemies()
{
	// Firing
	auto canShootIds = getCanShootEnemyIds();
	for (auto it = canShootIds.rbegin(); it != canShootIds.rend(); ++it) {
		const auto& enemy = enemies[*it];
		if (player.GetPosX() >= enemy.GetPosX() - 30 && player.GetPosX() <= enemy.GetPosX() + 30)
		{
			// find empty projectile
			for (auto& proj : enemyProjectiles)
			{
				if (!proj.IsAlive())
				{
					proj = Projectile{ enemy.GetProjectileType(), enemy.GetPosX(), enemy.GetPosY() };
					break;
				}
			}
		}
	}

	// Movement
	// Enemies behave like a beehive, all go to same direction.
	// If any enemy reaches the X bound (0 or screen width), 
	// move all of them down and switch direction to the opposite.
	bool moveDown = false;
	bool moveRight = true;

	for (const auto& e : enemies)
	{
		if (!e.IsAlive())
		{
			continue;
		}
		// Enemy reached the left bound, switch direction to right.
		if (e.GetPosX() - 15 < 0)
		{
			moveRight = true;
			moveDown = true;
			break;
		}
		// Enemy reached the right bound, switch direction to left.
		if (e.GetPosX() + 15 > screenWidth - e.GetW())
		{
			moveDown = true;
			moveRight = false;
			break;
		}
	}

	for (auto& e : enemies)
	{
		if (moveDown)
		{
			e.MoveDown();
		}		
		moveRight ? e.MoveRight() : e.MoveLeft();
	}
}

int main(int argc, char* args[])
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	int frameStart = 0;
	int frameTime;

	// TODO: Check if it's worth for difficult modes
	std::srand(std::time(nullptr));	

	initSDL();

	int enemyActionDelay = 500;
	int enemyLastActionTick = 0;

	enum
	{
		SHIP_ENEMY_MOVE_RIGHT,
		SHIP_ENEMY_MOVE_LEFT,
		SHIP_ENEMY_MOVE_DOWN,
	};

	int shipEnemyMove = SHIP_ENEMY_MOVE_RIGHT;
	bool shipEnemyMoveDown = false;

	initGame();
	std::cout << "Game initialized" << std::endl;

	while (running)
	{
		///////////////// EVENT HANDLING /////////////////
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				std::cout << "Quit event" << std::endl;
				running = false;
			}
			else if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_LEFT: player.MoveLeft(); break;


				case SDLK_RIGHT: player.MoveRight(); break;
				case SDLK_SPACE:
					if (!playerProjectile.IsAlive())
					{
						Mix_PlayChannel(0, fireWaveSound, 0);
						playerProjectile.SetPos(player.GetPosX(), (player.GetPosY() - player.GetH()) - 2);
					}
					break;
				case SDLK_ESCAPE: paused = paused ? false : true; break;
				}
			}
		}

		if (paused)
		{
			continue;
		}
		//////////////////////////////////////////////////

		///////////////// UPDATE /////////////////		
		checkCollisions();
		
		for (auto& e : enemies)
		{
			e.Update();
		}

		// Update player projectile position
		if (playerProjectile.IsAlive())
		{
			playerProjectile.UpdatePos();
		}
		// Update enemy projectiles
		for (auto& proj : enemyProjectiles)
		{
			if (proj.IsAlive())
			{
				proj.UpdatePos();
			}
		}

		Uint64 ticks = SDL_GetTicks64();

		// Lateral movement
		if (ticks - enemyLastActionTick > enemyActionDelay)
		{
			// updateEnemies();
			// If any enemy it's on x limits (0 or 800) it changes the direction to the opposite side.
			shipEnemyMoveDown = false;
			for (const auto& e : enemies)
			{
				if (!e.IsAlive())
				{
					continue;
				}

				// MOVE TO RIGHT
				if (e.GetPosX() - 15 < 0)
				{
					std::cout << "Enemies to RIGHT" << std::endl;
					shipEnemyMove = SHIP_ENEMY_MOVE_RIGHT;
					shipEnemyMoveDown = true;
					break;
				}
				// MOVE TO LEFT
				else if (e.GetPosX() + 15 > screenWidth - e.GetW())
				{
					std::cout << "Enemies to LEFT" << std::endl;
					shipEnemyMove = SHIP_ENEMY_MOVE_LEFT;
					shipEnemyMoveDown = true;
					break;
				}
			}

			// Enemy firing		
			auto x = getCanShootEnemyIds();
			for (auto it = x.rbegin(); it != x.rend(); ++it) {
				const auto& enemy = enemies[*it];
				if (player.GetPosX() >= enemy.GetPosX() - 30 && player.GetPosX() <= enemy.GetPosX() + 30)
				{
					// find empty projectile
					for (auto& proj : enemyProjectiles)
					{
						if (!proj.IsAlive())
						{
							proj = Projectile{ enemy.GetProjectileType(), enemy.GetPosX(), enemy.GetPosY() };
							break;
						}
					}
				}
			}

			for (auto& e : enemies)
			{
				if (shipEnemyMoveDown)
				{
					e.MoveDown();
				}
				if (shipEnemyMove == SHIP_ENEMY_MOVE_LEFT)
				{
					e.MoveLeft();
				}
				else if (shipEnemyMove == SHIP_ENEMY_MOVE_RIGHT)
				{
					e.MoveRight();
				}
			}

			enemyLastActionTick = ticks;
		}
		/////////////////////////////////////////

		////////////// RENDER /////////////////// 		
		renderScore();
		renderLifes();
		renderObstacles();
		renderProjectiles();
		renderShips();

		// UI stuff
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
		SDL_Rect bottomBar{ 0, screenHeight - 50, screenWidth, 5 };
		SDL_RenderFillRect(renderer, &bottomBar);

		// Render all entity colliders
		if (debug)
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);

			// Player collider
			auto c = player.GetCollider();
			SDL_FRect dst{ c.x, c.y, c.w, c.h };
			SDL_RenderDrawRectF(renderer, &dst);

			// Enemy colliders
			for (const auto& e : enemies)
			{
				if (e.IsAlive())
				{
					auto c = e.GetCollider();
					SDL_FRect dst{ c.x, c.y, c.w, c.h };
					SDL_RenderDrawRectF(renderer, &dst);
				}
			}

			// Player projectile collider			
			if (playerProjectile.IsAlive())
			{
				auto c = playerProjectile.GetCollider();
				SDL_FRect dst{ c.x, c.y, c.w, c.h };
				SDL_RenderDrawRectF(renderer, &dst);
			}

			// Show enemies which can fire
			for (const auto& enemyIdx : getCanShootEnemyIds())
			{
				auto c = enemies[enemyIdx].GetCollider();
				SDL_FRect dst{ c.x, c.y, c.w, c.h };
				SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
				SDL_RenderDrawRectF(renderer, &dst);
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
		/////////////////////////////////////////

		// Frametime
		frameTime = SDL_GetTicks64() - frameStart;
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
	}

	Mix_FreeChunk(fireWaveSound);
	Mix_FreeChunk(invaderKilledSound);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
