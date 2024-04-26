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
	if (state == State::MENU)
	{
		if (isKeyDown(KEY_CODE_SPACE))
		{
			state = State::PLAYING;
			clearInputKeys();
			return;
		}
	}
	if (state == State::PLAYING)
	{
		if (isKeyDown(KEY_CODE_LEFT) && player.GetState() != ShipState::MOVE_LEFT)
		{
			player.SetState(ShipState::MOVE_LEFT);
		}
		else if (isKeyUp(KEY_CODE_LEFT) && player.GetState() == ShipState::MOVE_LEFT)
		{
			player.SetState(ShipState::IDLE);
		}
		if (isKeyDown(KEY_CODE_RIGHT) && player.GetState() != ShipState::MOVE_RIGHT)
		{
			player.SetState(ShipState::MOVE_RIGHT);
		}
		else if (isKeyUp(KEY_CODE_RIGHT) && player.GetState() == ShipState::MOVE_RIGHT)
		{
			player.SetState(ShipState::IDLE);
		}

		if (isKeyDown(KEY_CODE_SPACE))
		{
			if (!projectiles[0].IsActive())
			{
				std::cout << "Fire" << std::endl;
				platform::playSound(SOUND_PLAYER_FIRE);
				Point2D coord = { player.GetX() + player.GetW() / 2.2f, (player.GetY() - player.GetH()) - 2 };
				projectiles[0] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_PLAYER, coord, static_cast<float>(screenHeight) };

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
	if (paused || state == State::MENU)
	{
		return;
	}

	switch (state)
	{
	case State::END_GAME:
		Init();
		state = State::MENU;
		break;
	case State::PLAYER_DESTROYED:
		if (player.GetState() == ShipState::DESTROYING)
		{
			player.Update();
			return;
		}
		else if (player.GetState() == ShipState::DESTROYED && lifes > 0)
		{
			state = State::PLAYING;
			return;
		}
		else if (player.GetState() == ShipState::DESTROYED && lifes == 0)
		{
			state = State::END_GAME;
			return;
		}
	}

	player.Update();

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
				if (player.GetX() >= invaders[i].GetX() - 30 && player.GetX() <= invaders[i].GetX() + 30)
				{
					Point2D coord = { invaders[i].GetX() + invaders[i].GetW() / 2.2f, (invaders[i].GetY() + invaders[i].GetH()) };
					switch (invaders[i].GetType())
					{
					case SPRITE_ENEMY_0:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_0, coord, static_cast<float>(screenHeight) };
						break;
					case SPRITE_ENEMY_1:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_1, coord, static_cast<float>(screenHeight) };
						break;
					case SPRITE_ENEMY_2:
						projectiles[1] = ProjectileV2{ SPRITE_PROJECTILE_TYPE_2, coord, static_cast<float>(screenHeight) };
						break;
					}
				}
			}
		}
	}

	// Ufo
	if (!invaders[ufoIdx].IsActive() && !ufoAppeared)
	{
		invaders[ufoIdx].SetPos({ 0, 20 });
	}

	CheckCollisions();
}

[[nodiscard]] bool Game::HasToSwitchInvaderDir()
{
	for (const auto& i : invaders)
	{
		// Some invader reached X left limit
		if (i.GetX() <= 0 && i.GetState() == ShipState::MOVE_LEFT)
		{
			return true;
		}
		// Some invader reached X right limit
		else if (i.GetX() + i.GetW() >= screenWidth && i.GetState() == ShipState::MOVE_RIGHT)
		{
			return true;
		}
	}
	return false;
}

void Game::Render()
{
	if (paused)
	{
		RenderPauseMenu();
	}
	else if (state == State::MENU)
	{
		RenderMenu();
	}
	else
	{
		player.Render();

		RenderScore();
		RenderLifes();

		for (auto& o : obstacles)
		{
			o.Render();
		}

		for (const auto& p : projectiles)
		{
			p.Render();
		}

		for (const auto& i : invaders)
		{
			i.Render();
		}
		
		Rect bottomBar{ 0, screenHeight - 50, screenWidth, 5 };
		platform::addBuffRect(bottomBar, 0xff0000, true);


		/////////////////// DEBUG ///////////////////
		//for (const auto& pair : invaderCanShoot)
		//{
		//	if (pair.second == true)
		//	{
		//		Rect collider = invaders[pair.first].GetCollider();
		//		platform::addBuffRect(collider, 0xff000000, false);
		//	}
		//}
		/////////////////////////////////////////////
	}

	platform::renderBuff();
	platform::clearBuff();
}

void Game::DestroyInvader(const int idx)
{
	invaders[idx].SetState(ShipState::DESTROYING);
	score += invaders[idx].GetScore();
	if (idx >= 11)
	{
		invaderCanShoot[idx - 11] = true;
	}
}

void Game::CheckCollisions()
{
	for (auto& p : projectiles)
	{
		if (!p.IsActive())
		{
			continue;
		}

		auto projColl = p.GetCollider();
		auto playerColl = player.GetCollider();

		if (projColl.intersects(playerColl))
		{
			--lifes;			
			player.SetState(ShipState::DESTROYING);
			p.Destroy();
			state = State::PLAYER_DESTROYED;
			return;
		}

		for (int i = 0; i < invaders.size(); i++)
		{	
			if (!invaders[i].IsActive())
			{
				continue;
			}

			if (projColl.intersects(invaders[i].GetCollider()))
			{				
				p.Destroy();
				DestroyInvader(i);
			}
		}
	}
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
		invaders[i] = Invader{ SPRITE_ENEMY_0, {x, y} };
	}
	lastY += yOffset;

	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;		
		invaders[i + 11] = Invader{ SPRITE_ENEMY_1, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;		
		invaders[i + 22] = Invader{ SPRITE_ENEMY_1, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;		
		invaders[i + 33] = Invader{ SPRITE_ENEMY_2, {x, y} };
	}

	lastY += yOffset;
	for (int i = 0; i < 11; i++)
	{
		float x = i * (enemySizeX);
		float y = lastY + yOffset;		
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

	player.screenWidth = screenWidth;
	player.screenHeight = screenHeight;
	player.SetPos(Point2D{ screenWidth / 2 - player.GetW(), static_cast<float>(screenHeight) - 100 });
	invaders[ufoIdx].screenWidth = screenWidth;
	invaders[ufoIdx] = Invader{ SPRITE_ENEMY_UFO, { -1, -1 } };	
}