#include "entities.h"
#include "platform.h"
#include "input.h"
#include "audio.h"

#include <iostream>
#include <array>
#include <cstdlib>
#include <vector>
#include <cassert>

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

void handleInput();
void update();
void render();

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
				platform::playSound(SOUND_INVADER_DESTROYED);
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
	const int FPS = 30;
	const int frameDelay = 1000 / FPS;

	int frameStart = 0;
	int frameTime;

	initGame();

	platform::createWindow(screenWidth, screenHeight);	

	while (running)
	{
		frameStart = platform::getTicks();

		if (paused)
		{
			continue;
		}

		handleInput();
		update();
		render();		

		platform::clearBuff();
		clearInputKeys();

		// TODO: Move logic to sdl_platform.cpp
		// Frametime
		/*frameTime = SDL_GetTicks64() - frameStart;
		if (frameDelay > frameTime)
		{
			
			SDL_Delay(frameDelay - frameTime);
		}*/
		do {
			frameStart = platform::getTicks();			
		} while (frameDelay > frameStart);
	}

	platform::exit();
	return 0;
}

void handleInput()
{
	if (isKeyDown(KEY_CODE_LEFT))
	{
		std::cout << "moving to the left" << std::endl;
		player.MoveLeft();
	}
	else if (isKeyDown(KEY_CODE_RIGHT))
	{
		player.MoveRight();
	}
	else if (isKeyDown(KEY_CODE_SPACE))
	{
		platform::playSound(SOUND_PLAYER_FIRE);
		playerProjectile.SetPos(player.GetPosX(), (player.GetPosY() - player.GetH()) - 2);
	}
	else if (isKeyDown(KEY_CODE_ESCAPE))
	{
		std::cout << "************" << std::endl;
		paused = paused ? false : true;
	}
}

void update()
{
	platform::updateWindow();

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

	// Uint64 ticks = SDL_GetTicks64();
	uint64_t ticks = getTicks();

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
}

void renderScore()
{
	auto thousands = score / 1000;
	auto hundreds = (score % 1000) / 100;
	auto tens = (score % 100) / 10;
	auto units = (score % 10);

	auto digitTextures = {
		spriteNumbers[thousands],
		spriteNumbers[hundreds],
		spriteNumbers[tens],
		spriteNumbers[units],
	};

	int w = spriteFontW * spriteFontScaleX;
	int h = spriteFontY * spriteFontScaleY;
	Rect dst{ 0, 0, w, h };
	for (const auto& dt : digitTextures)
	{
		platform::addBuffTexture(dt, dst);
		dst.x += w;
	}
}

void renderLifes()
{
	Rect numDst{ 20, screenHeight - 40, spriteFontW * spriteFontScaleX, spriteFontY * spriteFontScaleY };
	platform::addBuffTexture(spriteNumbers[lifes], numDst);

	Rect dst{ 60, screenHeight - 40, player.GetW(), player.GetH() };
	for (int i = 0; i < lifes; i++)
	{
		platform::addBuffTexture(player.GetTexture(), dst);
		dst.x += player.GetW();
	}
}

void renderObstacles()
{
	for (const auto& o : obstacles)
	{
		if (o.IsAlive())
		{
			Rect dst{ o.GetPosX(), o.GetPosY(), o.GetW(), o.GetH() };
			platform::addBuffTexture(o.GetTexture(), dst);
		}
	}
}

void renderProjectiles()
{
	// Render player projectile
	if (playerProjectile.IsAlive())
	{
		Rect dst{ playerProjectile.GetPosX(), playerProjectile.GetPosY(), playerProjectile.GetW(), playerProjectile.GetH() };
		platform::addBuffTexture(playerProjectile.GetTexture(), dst);
	}

	// Render enemy projectiles
	for (const auto& proj : enemyProjectiles)
	{
		if (proj.IsAlive())
		{
			Rect dst{ proj.GetPosX(), proj.GetPosY(), proj.GetW(), proj.GetH() };
			platform::addBuffTexture(proj.GetTexture(), dst);
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
			Rect dst{ e.GetPosX(), e.GetPosY(), e.GetW(), e.GetH() };
			platform::addBuffTexture(e.GetTexture(), dst);
		}
	}

	Rect dst{ player.GetPosX(), player.GetPosY(), player.GetW(), player.GetH() };
	platform::addBuffTexture(player.GetTexture(), dst);
}

void render()
{
	renderScore();
	renderLifes();
	renderObstacles();
	renderProjectiles();
	renderShips();
	Rect bottomBar{ 0, screenHeight - 50, screenWidth, 5 };
	platform::addBuffRect(bottomBar, 0xff0000);
	platform::renderBuff();
}