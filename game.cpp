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
	srand(NULL);
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
		if (isKeyDown(KEY_CODE_LEFT) && playerv2.GetState() != PlayerState::MOVE_LEFT)
		{
			playerv2.SetState(PlayerState::MOVE_LEFT);
		}
		else if (isKeyUp(KEY_CODE_LEFT) && playerv2.GetState() == PlayerState::MOVE_LEFT)
		{
			playerv2.SetState(PlayerState::IDLE);
		}
		if (isKeyDown(KEY_CODE_RIGHT) && playerv2.GetState() != PlayerState::MOVE_RIGHT)
		{
			playerv2.SetState(PlayerState::MOVE_RIGHT);
		}
		else if (isKeyUp(KEY_CODE_RIGHT) && playerv2.GetState() == PlayerState::MOVE_RIGHT)
		{
			playerv2.SetState(PlayerState::IDLE);
		}

		if (isKeyDown(KEY_CODE_SPACE))
		{
			if (!projectiles[0].IsActive())
			{
				std::cout << "Fire" << std::endl;
				platform::playSound(SOUND_PLAYER_FIRE);
				Point2D coord = { playerv2.GetX() + player.GetW() / 2.2f, (playerv2.GetY() - playerv2.GetH()) - 2 };
				projectiles[0] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_PLAYER, coord };

			}
		}
		// else if (isKeyUp(KEY_CODE_RIGHT) && playerv2.GetState() == PlayerState::MOVE_RIGHT)
		// {
		// 	playerv2.SetState(PlayerState::IDLE);
		// }

		//std::cout << "Playing" << std::endl;
		//if (isKeyDown(KEY_CODE_LEFT))
		//{
		//	std::cout << "Move left" << std::endl;
		//	// player.MoveLeft();
		//	playerv2.SetState(PlayerState::MOVE_LEFT);
		//} 
		//if (isKeyDown(KEY_CODE_RIGHT))
		//{
		//	// player.MoveRight();
		//}
		//if (isKeyDown(KEY_CODE_SPACE))
		//{
		//	// if (!playerProjectile.IsAlive())
		//	// {
		//	// 	platform::playSound(SOUND_PLAYER_FIRE);
		//	// 	playerProjectile.SetPos(player.GetPosX() + player.GetW() / 2.2f, (player.GetPosY() - player.GetH()) - 2);
		//	// }
		//}
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

	playerv2.Update();

	for (auto& p : projectiles)
	{
		p.Update();
	}
	
	if (Game::HasToSwitchInvaderDir())
	{
		for (auto& i : invaders)
		{			
			i.SwitchDirection();
			i.Update();		
		}
	}
	else
	{
		//// Find empty projectile
		//int freeProjIdx = -1;
		//for (int i = 1; i < projectiles.size(); i++)
		//{			
		//	if (!projectiles[i].IsActive())
		//	{
		//		std::cout << "Free: " << i << std::endl;
		//		freeProjIdx = i;
		//		break;
		//	}
		//}
		// Update invaders pos and fire if can
		for (int i = 0; i < invaders.size(); i++)
		{
			invaders[i].Update();

			if (invaderCanShoot[i] && !projectiles[1].IsActive())
			{
				// Player is in front
				if (playerv2.GetX() >= invaders[i].GetX() - 30 && playerv2.GetX() <= invaders[i].GetX() + 30)
				{
					Point2D coord = { invaders[i].GetX() + invaders[i].GetW() / 2.2f, (invaders[i].GetY() - invaders[i].GetH()) - 2 };
					switch (invaders[i].GetType())
					{
					case SPRITE_ENEMY_0:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_0, coord };
						break;
					case SPRITE_ENEMY_1:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_1, coord };
						break;
					case SPRITE_ENEMY_2:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_2, coord };
						break;
					}
				}
			}
		}
	}
}

[[nodiscard]] bool Game::HasToSwitchInvaderDir()
{
	for (const auto& i : invaders)
	{
		// Some invader reached X left limit
		if (i.GetX() <= 0 && i.GetState() == InvaderState::MOVE_LEFT)
		{
			return true;
		}
		// Some invader reached X right limit
		else if (i.GetX() + i.GetW() >= screenWidth && i.GetState() == InvaderState::MOVE_RIGHT)
		{
			return true;
		}
	}
	return false;
}

void Game::OldUpdate()
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

	player.Update();

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

	// Spawn ufo
	// if (std::rand() / ((RAND_MAX + 1u) / 20) == 20 && !enemyUfo.IsAlive() && !ufoAppeared)	
	if (!enemyUfo.IsAlive() && !ufoAppeared)
	{
		enemyUfo = Enemy{ SPRITE_ENEMY_UFO, 0, 20 };
		ufoAppeared = true;
		std::cout << "Spaw ufo" << std::endl;

		// platform::playSound(SOUND_UFO);
	}
	// Ufo logic
	if (enemyUfo.IsAlive())
	{
		enemyUfo.Update();
		enemyUfo.MoveRight2();
		platform::playSound(SOUND_UFO);

		if (enemyUfo.GetPosX() >= spriteShipWidth + screenWidth)
		{
			enemyUfo.Destroy();
		}
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
		playerv2.Render();

		RenderScore();
		RenderLifes();

		//for (auto& o : obstacles)
		//{
		//	o.Render();
		//}

		for (const auto& p : projectiles)
		{
			p.Render();
		}

		for (const auto& i : invaders)
		{
			i.Render();
		}

		//RenderObstacles();
		//RenderProjectiles();
		//RenderShips();
		Rect bottomBar{ 0, screenHeight - 50, screenWidth, 5 };
		platform::addBuffRect(bottomBar, 0xff0000, true);


		/////////////////// DEBUG ///////////////////
		for (const auto& pair : invaderCanShoot)
		{
			if (pair.second == true)
			{				
				Rect collider = invaders[pair.first].GetCollider();
				platform::addBuffRect(collider, 0xff000000, false);
			}
		}
		/////////////////////////////////////////////
	}

	platform::renderBuff();
	platform::clearBuff();
}

void Game::CheckCollisions()
{
	// Check if some enemy collides with player or player projectile
	//if (playerProjectile.IsAlive())
	//{
	//	for (auto& e : enemies)
	//	{
	//		if (!e.IsAlive())
	//		{
	//			continue;
	//		}

	//		if (playerProjectile.GetCollider().intersects(e.GetCollider()))
	//		{
	//			platform::playSound(SOUND_INVADER_DESTROYED);
	//			std::cout << "** Enemy destroyed **" << std::endl;
	//			e.Die();
	//			playerProjectile.Destroy();
	//			score += e.GetScore();
	//			break;
	//		}
	//		else if (player.GetCollider().intersects(e.GetCollider()))
	//		{
	//			std::cout << "YOU DIE" << std::endl;
	//			running = false;
	//			break;
	//		}
	//	}

	//	for (auto& o : obstacles)
	//	{
	//		if (!o.IsAlive())
	//		{
	//			continue;
	//		}

	//		Rect collision = playerProjectile.GetCollider().intersectRect(o.GetCollider());
	//		if (!collision.IsZero())
	//		{
	//			o.ReceiveProjectile({ collision.x, collision.y });
	//			playerProjectile.Destroy();
	//			break;
	//		}
	//	}

	//	for (auto& p : enemyProjectiles)
	//	{
	//		if (playerProjectile.GetCollider().intersects(p.GetCollider()))
	//		{
	//			p.Destroy();
	//			playerProjectile.Destroy();
	//			break;
	//		}
	//	}

	//	if (enemyUfo.IsAlive())
	//	{
	//		if (playerProjectile.GetCollider().intersects(enemyUfo.GetCollider()))
	//		{
	//			std::cout << "ufo reacheeeed!" << std::endl;
	//			enemyUfo.Die();
	//			playerProjectile.Destroy();
	//			score += enemyUfo.GetScore();
	//		}
	//	}
	//}

	//// Check if some enemy projectile collides with player or obstacles
	//for (auto& proj : enemyProjectiles)
	//{
	//	if (!proj.IsAlive())
	//	{
	//		continue;
	//	}

	//	// Check if projectile has reached player
	//	if (proj.GetCollider().intersects(player.GetCollider()))
	//	{
	//		if (--lifes == 0)
	//		{
	//			std::cout << "YOU DIE" << std::endl;
	//			stage = Stage::END_GAME;
	//		}
	//		player.Die();
	//		proj.Destroy();
	//		break;
	//	}

	//	// Check if projectile collides with any obstacle
	//	for (auto& o : obstacles)
	//	{
	//		if (!o.IsAlive())
	//		{
	//			continue;
	//		}
	//		Rect collision = proj.GetCollider().intersectRect(o.GetCollider());
	//		if (!collision.IsZero())
	//		{
	//			o.ReceiveProjectile({ collision.x, collision.y });
	//			proj.Destroy();
	//			break;
	//		}
	//	}
	//}
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
	Rect src{ spritePlayer[0].x, spritePlayer[0].y, spriteShipWidth, spriteShipHeight };
	for (int i = 0; i < lifes; i++)
	{
		platform::addBuffTexture(src, dst);
		dst.x += player.GetW();
	}
}

void Game::RenderShips()
{
	//	// Render enemies
	//	for (const auto& e : enemies)
	//	{
	//		if (e.IsAlive())
	//		{
	//			Rect dst{ e.GetPosX(), e.GetPosY(), e.GetW(), e.GetH() };
	//			platform::addBuffTexture(e.GetTexture(), dst);
	//		}
	//	}
	//
	//	// Render player
	//	Rect dst{ player.GetPosX(), player.GetPosY(), player.GetW(), player.GetH() };
	//	platform::addBuffTexture(player.GetTexture(), dst);
	//
	//	// Render ufo
	//	if (enemyUfo.IsAlive())
	//	{
	//		Rect dst{ enemyUfo.GetPosX(), enemyUfo.GetPosY(), enemyUfo.GetW(), enemyUfo.GetH() };
	//		platform::addBuffTexture(enemyUfo.GetTexture(), dst);
	//	}
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

		invaders[i] = Invader{ SPRITE_ENEMY_0, {x, y} };
	}
	lastY += yOffset;

	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 11] = Enemy{ SPRITE_ENEMY_1, x, y };

		invaders[i + 11] = Invader{ SPRITE_ENEMY_1, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 22] = Enemy{ SPRITE_ENEMY_1, x, y };

		invaders[i + 22] = Invader{ SPRITE_ENEMY_1, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 33] = Enemy{ SPRITE_ENEMY_2, x, y };

		invaders[i + 33] = Invader{ SPRITE_ENEMY_2, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;
		enemies[i + 44] = Enemy{ SPRITE_ENEMY_2, x, y };

		invaders[i + 44] = Invader{ SPRITE_ENEMY_2, {x, y} };
		invaderCanShoot[i + 44] = true;
	}

	float obstacleXOffset = 0;

	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i] = Obstacle{ i * 200 + obstacleXOffset, (float)screenHeight - 150 };
	}

	score = 0;
	lifes = 3;

	playerv2.screenWidth = screenWidth;
	playerv2.screenHeight = screenHeight;
	playerv2.SetPos(Point2D{ screenWidth / 2 - playerv2.GetW(), static_cast<float>(screenHeight) - 100 });
}