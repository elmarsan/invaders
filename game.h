#pragma once

#include "entities.h"
#include <vector>

enum
{
	SHIP_ENEMY_MOVE_RIGHT,
	SHIP_ENEMY_MOVE_LEFT,
	SHIP_ENEMY_MOVE_DOWN,
};

enum class Stage
{
	PLAYING,
	PAUSED,
	MENU,
	END_GAME
};

class Game
{
public:
	Game(int screenWidth, int screenHeight);

	void Run();

private:
	void HandleInput();
	void Update();
	void Render();
	void RenderScore();
	void RenderLifes();
	void RenderObstacles();
	void RenderProjectiles();
	void RenderShips();
	void RenderPauseMenu();
	void RenderMenu();
	void CheckCollisions();

	void Init();

	[[nodiscard]] std::vector<int> GetCanShootEnemyIds();

private:
	Stage stage = Stage::MENU;
	Player player;
	Projectile playerProjectile{};
	std::array<Enemy, 55> enemies;
	std::array<Obstacle, 4> obstacles{};
	std::array<Projectile, 1> enemyProjectiles{};

	int screenWidth;
	int screenHeight;

	int enemyActionDelay = 500;
	int enemyLastActionTick = 0;
	int shipEnemyMove = SHIP_ENEMY_MOVE_RIGHT;
	bool shipEnemyMoveDown = false;
	int score = 0;
	int lifes = 3;
};