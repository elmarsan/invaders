#include "game.h"
#include "platform.h"
#include "input.h"
#include "audio.h"
#include "util.h"

bool running = true;
bool paused = false;
bool muteSound = false;

Game::Game(int screenWidth, int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight)
{
	Init();
}

void Game::Run()
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	uint64_t frameStart = 0;
	int frameTime;

	platform::createWindow(screenWidth, screenHeight);

	while (running)
	{
		platform::updateWindow();
		frameStart = platform::getTicks();

		HandleInput();
		Update();
		Render();

		do {
			frameTime = platform::getTicks() - frameStart;
		} while (frameDelay > frameTime);
	}

	platform::exit();
}

void Game::HandleInput()
{
	if (stage == Stage::MENU)
	{
		if (isKeyDown(KEY_CODE_SPACE))
		{
			stage = Stage::PLAYING;
			clearInputKeys();
			return;
		}
	}
	if (stage == Stage::PLAYING)
	{
		if (isKeyDown(KEY_CODE_LEFT))
		{
			player.MoveLeft();
		}
		if (isKeyDown(KEY_CODE_RIGHT))
		{
			player.MoveRight();
		}
		if (isKeyDown(KEY_CODE_SPACE))
		{
			if (!playerProjectile.IsAlive())
			{
				platform::playSound(SOUND_PLAYER_FIRE);
				playerProjectile.SetPos(player.GetPosX(), (player.GetPosY() - player.GetH()) - 2);
			}
		}
	}
	if (isKeyDown(KEY_CODE_ESCAPE))
	{
		paused = !paused;
	}
}

void Game::Update()
{
	if (paused || stage == Stage::MENU)
	{
		return;
	}

	if (stage == Stage::END_GAME)
	{
		Init();
		stage = Stage::MENU;
	}

	CheckCollisions();

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

	uint64_t ticks = platform::getTicks();
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
				shipEnemyMove = SHIP_ENEMY_MOVE_RIGHT;
				shipEnemyMoveDown = true;
				break;
			}
			// MOVE TO LEFT
			else if (e.GetPosX() + 15 > screenWidth - e.GetW())
			{
				shipEnemyMove = SHIP_ENEMY_MOVE_LEFT;
				shipEnemyMoveDown = true;
				break;
			}
		}

		// Enemy firing		
		auto x = GetCanShootEnemyIds();
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
			if (!e.IsAlive())
			{
				continue;
			}
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
}

void Game::Render()
{
	if (paused)
	{
		RenderPauseMenu();
	}
	else if (stage == Stage::MENU)
	{
		RenderMenu();
	}
	else
	{
		RenderScore();
		RenderLifes();
		RenderObstacles();
		RenderProjectiles();
		RenderShips();
		Rect bottomBar{ 0, screenHeight - 50, screenWidth, 5 };
		platform::addBuffRect(bottomBar, 0xff0000);
	}

	platform::renderBuff();
	platform::clearBuff();
}

void Game::CheckCollisions()
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

			if (playerProjectile.GetCollider().intersects(e.GetCollider()))
			{
				platform::playSound(SOUND_INVADER_DESTROYED);
				std::cout << "** Enemy destroyed **" << std::endl;
				e.Die();
				playerProjectile.Destroy();
				score += e.GetScore();
				break;
			}
			else if (player.GetCollider().intersects(e.GetCollider()))
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
		if (!proj.IsAlive())
		{
			continue;
		}

		// Check if projectile has reached player
		if (proj.GetCollider().intersects(player.GetCollider()))
		{
			if (--lifes == 0)
			{
				std::cout << "YOU DIE" << std::endl;
				stage = Stage::END_GAME;
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

std::vector<int> Game::GetCanShootEnemyIds()
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

void Game::RenderScore()
{
	std::string coreStr = std::to_string(score);
	std::string formattedScore = std::string(4 - coreStr.length(), '0') + coreStr;
	Point2D dstPoint{ 0, 0 };
	platform::addBuffFont(dstPoint, formattedScore);
}

void Game::RenderLifes()
{
	Point2D dstPoint{ 20, screenHeight - 40 };
	platform::addBuffFont(dstPoint, std::to_string(lifes));

	Rect dst{ 60, screenHeight - 40, player.GetW(), player.GetH() };
	for (int i = 0; i < lifes; i++)
	{
		platform::addBuffTexture(player.GetTexture(), dst);
		dst.x += player.GetW();
	}
}

void Game::RenderObstacles()
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

void Game::RenderProjectiles()
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

void Game::RenderShips()
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

void Game::RenderPauseMenu()
{
	std::string text{ "PAUSE" };

	// Get middle of screen
	auto winBarDims = platform::getWindowBarDims();
	int w = spriteFontWidth * spriteScaleX;
	int h = spriteFontHeight * spriteScaleY;
	int x = (screenWidth / 2) - (w * text.size()) / 2;
	int y = (screenHeight / 2) - (h * text.size()) / 2 + winBarDims.x;

	const Point2D dstPoint{ x, y };
	platform::addBuffFont(dstPoint, text);
}

void Game::RenderMenu()
{
	auto winBarDims = platform::getWindowBarDims();
	int w = spriteFontWidth * spriteScaleX;
	int h = spriteFontHeight * spriteScaleY;

	std::string text{ "SPACE INVADERS" };
	int x = (screenWidth / 2) - (w * text.size()) / 2;
	int y = 100;

	Point2D dstPoint{ x, y };
	platform::addBuffFont(dstPoint, text);

	text = "PRESS SPACE TO PLAY";
	dstPoint.x = (screenWidth / 2) - (w * text.size()) / 2;
	dstPoint.y += h * 2;
	platform::addBuffFont(dstPoint, text);

	text = "*SCORE TABLE*";
	dstPoint.x = (screenWidth / 2) - (w * text.size()) / 2 - 30;
	dstPoint.y += 120;
	platform::addBuffFont(dstPoint, text);

	text = "= 300 POINTS";
	dstPoint.x = (screenWidth / 2) - (w * text.size()) / 2;
	dstPoint.y += h * 2.2;
	Rect srcRect{ spriteEnemyUfo[0].x, spriteEnemyUfo[0].y, spriteShipWidth, spriteShipHeight };
	Rect dstRect{ dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
	platform::addBuffTexture(srcRect, dstRect);
	platform::addBuffFont(dstPoint, text);

	dstPoint.y += h * 2.2;
	text = "= 30 POINTS  ";
	srcRect = { spriteEnemy0[0].x, spriteEnemy0[0].y, spriteShipWidth, spriteShipHeight };
	dstRect = { dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
	platform::addBuffTexture(srcRect, dstRect);
	platform::addBuffFont(dstPoint, text);

	dstPoint.y += h * 2.2;
	text = "= 20 POINTS  ";
	srcRect = { spriteEnemy1[0].x, spriteEnemy1[0].y, spriteShipWidth, spriteShipHeight };
	dstRect = { dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
	platform::addBuffTexture(srcRect, dstRect);
	platform::addBuffFont(dstPoint, text);

	dstPoint.y += h * 2.2;
	text = "= 10 POINTS  ";
	srcRect = { spriteEnemy2[0].x, spriteEnemy2[0].y, spriteShipWidth, spriteShipHeight };
	dstRect = { dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
	platform::addBuffTexture(srcRect, dstRect);
	platform::addBuffFont(dstPoint, text);
}

void Game::Init()
{
	// Check where all constants go
	float enemySizeX = 16 * spriteScaleX;

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

	score = 0;
	lifes = 3;
}