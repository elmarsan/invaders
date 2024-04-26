#include "game.h"
#include "platform.h"
#include "input.h"
#include "audio.h"
#include "util.h"
#include "asset.h"

#include <random>

bool running = true;
bool paused = false;

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

		if (isKeyDown(KEY_CODE_SPACE) && enableSpaceKey)
		{
			if (!projectiles[0].IsActive())
			{
				platform::playSound(SOUND_PLAYER_FIRE, 0);
				Point2D coord = { player.GetX() + player.GetW() / 2.2f, (player.GetY() - player.GetH()) - 2 };
				projectiles[0] = Projectile{ SPRITE_PROJECTILE_TYPE_PLAYER, coord, static_cast<float>(screenHeight) };
			}			
			enableSpaceKey = false;
		} 
		else if (isKeyUp(KEY_CODE_SPACE))
		{
			enableSpaceKey = true;
		}
	}
	if (isKeyDown(KEY_CODE_ESCAPE) && enableEscapeKey)
	{
		paused = !paused;
		enableEscapeKey = false;
	}
	else if (isKeyUp(KEY_CODE_ESCAPE) && !enableEscapeKey)
	{
		enableEscapeKey = true;
	}
	if (isKeyDown(KEY_CODE_M) && enableMKey)
	{
		platform::toggleSound();
		enableMKey = false;
	} 
	else if (isKeyUp(KEY_CODE_M) && !enableMKey)
	{
		enableMKey = true;
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
			projectiles.fill(Projectile{});
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
	// Update invaders pos and fire if can
	else
	{
		int activeCount = 0;

		// Update positions
		for (auto& i : invaders)
		{
			i.Update();

			if (i.IsActive())
			{
				activeCount++;
			}
		}

		// Win
		if (activeCount == 0)
		{
			state = State::MENU;
		}

		// Find empty projectile idxs
		std::vector<int> freeProjIdxs;
		for (int i = 1; i < projectiles.size(); i++)
		{
			if (!projectiles[i].IsActive())
			{
				freeProjIdxs.push_back(i);
			}
		}

		// If projectiles available then fire
		if (!freeProjIdxs.empty())
		{
			for (int i = 0; i < invaders.size(); i++)
			{
				if (!invaders[i].IsActive())
				{
					continue;
				}

				if (freeProjIdxs.empty())
				{
					break;
				}

				// The invader can shoot if there are no enemies in front of him
				if (invaderCanShoot[i])
				{
					// Player is in front
					if (player.GetX() >= invaders[i].GetX() - 30 && player.GetX() <= invaders[i].GetX() + 30)
					{
						InvaderFire(i, freeProjIdxs.back());
						freeProjIdxs.pop_back();
					}
					// Check for in front obstacles
					else
					{
						for (const auto& o : obstacles)
						{
							if (!o.IsActive())
							{
								continue;
							}

							// Obstacle in front
							if (o.GetX() >= invaders[i].GetX() - 30 && o.GetX() <= invaders[i].GetX() + 30)
							{
								InvaderFire(i, freeProjIdxs.back());
								freeProjIdxs.pop_back();
							}
						}
					}
				}
			}
		}
	}

	// Spawn ufo
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(1, 1000);
	int random_num = dis(gen);
	if (!invaders[ufoIdx].IsActive() && !ufoAppeared && random_num == 1000)
	{
		invaders[ufoIdx].SetPos({ 0, 20 });
		invaders[ufoIdx].screenWidth = screenWidth;
		ufoAppeared = true;
	}

	CheckCollisions();
}

[[nodiscard]] bool Game::HasToSwitchInvaderDir()
{
	for (int i = 0; i < invaders.size() - 1; i++)
	{
		// Some invader reached X left limit
		if (invaders[i].GetX() <= 0 && invaders[i].GetState() == ShipState::MOVE_LEFT)
		{
			return true;
		}
		// Some invader reached X right limit
		else if (invaders[i].GetX() + invaders[i].GetW() >= screenWidth && invaders[i].GetState() == ShipState::MOVE_RIGHT)
		{
			return true;
		}
		// Y limit reached, restart game
		else if (invaders[i].GetY() >= screenHeight - 50)
		{
			state = State::END_GAME;
		}
	}
	return false;
}

// Fire projectile from invader by given idx using projectile based on projIdx.
void Game::InvaderFire(const int idx, const int projIdx)
{
	Point2D coord = { invaders[idx].GetX() + invaders[idx].GetW() / 2.2f, (invaders[idx].GetY() + invaders[idx].GetH() + 0.5f) };
	switch (invaders[idx].GetType())
	{
	case SPRITE_ENEMY_0:
		projectiles[projIdx] = Projectile{ SPRITE_PROJECTILE_TYPE_0, coord, static_cast<float>(screenHeight) };
		break;
	case SPRITE_ENEMY_1:
		projectiles[projIdx] = Projectile{ SPRITE_PROJECTILE_TYPE_1, coord, static_cast<float>(screenHeight) };
		break;
	case SPRITE_ENEMY_2:
		projectiles[projIdx] = Projectile{ SPRITE_PROJECTILE_TYPE_2, coord, static_cast<float>(screenHeight) };
		break;
	}
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
	}

	platform::renderBuff();
	platform::clearBuff();
}

void Game::DestroyInvader(const int idx)
{
	platform::playSound(SOUND_INVADER_DESTROYED, 2);
	invaders[idx].SetState(ShipState::DESTROYING);
	score += invaders[idx].GetScore();
	if (idx >= 11)
	{
		invaderCanShoot[idx - 11] = true;
	}
}

void Game::CheckCollisions()
{
	// Check collison of enemy projectiles with player projectiles and obstacles
	for (int i = 1; i < projectiles.size(); i++)
	{
		if (projectiles[i].GetCollider().intersects(projectiles[0].GetCollider()))
		{
			projectiles[0].Destroy();
			projectiles[i].Destroy();
		}
	}

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
				return;
			}
		}

		for (auto& o : obstacles)
		{
			Rect collision = projColl.intersectRect(o.GetCollider());
			if (!collision.IsZero())
			{
				o.ReceiveProjectile({ collision.x, collision.y });
				p.Destroy();
				break;
			}
		}
	}

	// Invader with obstacle and player
	for (auto& i : invaders)
	{
		if (!i.IsActive())
		{
			continue;
		}

		if (i.GetCollider().intersects(player.GetCollider()))
		{
			state = State::END_GAME;
			return;
		}

		for (auto& o : obstacles)
		{
			if (!o.IsActive())
			{
				continue;
			}

			if (o.GetCollider().intersects(i.GetCollider()))
			{
				state = State::END_GAME;
				return;
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

	text = "= 300 POINTS";
	dstPoint.x = (screenWidth / 2) - (w * text.size()) / 2;
	dstPoint.y += h * 2.2;
	Rect srcRect{ spriteEnemyUfo[0].x, spriteEnemyUfo[0].y, spriteShipWidth, spriteShipHeight };
	Rect dstRect{ dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
	platform::addBuffTexture(srcRect, dstRect);
	platform::addBuffFont(dstPoint, text);

	static std::vector<std::string> scores{
		"= 30 POINTS  ",
		"= 20 POINTS  ",
		"= 10 POINTS  "
	};
	for (const auto& t : scores)
	{
		dstPoint.y += h * 2.2;
		srcRect = { spriteEnemy0[0].x, spriteEnemy0[0].y, spriteShipWidth, spriteShipHeight };
		dstRect = { dstPoint.x - 60, dstPoint.y, spriteShipWidth * spriteScaleX, spriteShipHeight * spriteScaleY };
		platform::addBuffTexture(srcRect, dstRect);
		platform::addBuffFont(dstPoint, t);
	}
	
	static std::vector<std::string> controls{
		"PRESS SPACE TO FIRE",
		"PRESS M TO MUTE",
		"PRESS ESC TO PAUSE",
	};

	dstPoint.x = (screenWidth / 2) - (w * controls[0].size()) / 2;
	dstPoint.y += h * 2 + 100;
	for (const auto& t : controls)
	{
		platform::addBuffFont(dstPoint, t);
		dstPoint.y += 40;
	}
}

void Game::Init()
{	
	float enemySizeX = spriteShipWidth * spriteScaleX;	
	float y = 50;

	int enemyType = SPRITE_ENEMY_0;
	for (int i = 0; i < 5; i++)
	{
		if (i == 1 || i == 2)
		{
			enemyType = SPRITE_ENEMY_1;
		}
		else if (i == 3 || i == 4)
		{
			enemyType = SPRITE_ENEMY_2;
		}

		for (int j = 0; j < 11; j++)
		{
			int idx = j + i * 11;			
			invaders[idx] = Invader{ enemyType, {j * (enemySizeX), y} };			
			if (idx >= 44)
			{
				invaderCanShoot[idx] = true;
			}
		}		
		y += 50;
	}

	for (int i = 0; i < obstacles.size(); i++)
	{
		obstacles[i] = Obstacle{ {i * 200.0f + 50.0f, screenHeight - 150.0f} };
	}

	score = 0;
	lifes = 3;
	player.screenWidth = screenWidth;
	player.screenHeight = screenHeight;
	player.SetPos(Point2D{ screenWidth / 2 - player.GetW(), static_cast<float>(screenHeight) - 100.0f });
	invaders[ufoIdx] = Invader{ SPRITE_ENEMY_UFO, { -1, -1 } };
}