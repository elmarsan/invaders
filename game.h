#pragma once

#include "entities.h"
#include "player.h"
#include "projectile.h"
#include "invader.h"
#include <unordered_map>

enum class State
{
	PLAYING,
	PAUSED,
	MENU,
	END_GAME,
	PLAYER_DESTROYED
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
	void RenderShips();
	void RenderPauseMenu();
	void RenderMenu();
	void CheckCollisions();

	void Init();	
	
	[[nodiscard]] bool HasToSwitchInvaderDir();

	void DestroyInvader(const int idx);

private:	
	PlayerV2 player{0, 0};
	std::array<ProjectileV2, 2> projectiles;
	std::array<Invader, 56> invaders;
	int ufoIdx = 55;
	bool ufoAppeared = false;
	std::unordered_map<int, bool> invaderCanShoot;		
	State state = State::MENU;		
	int screenWidth = 0;
	int screenHeight = 0;
	int score = 0;
	int lifes = 3;	

	std::array<Obstacle, 4> obstacles{};
};
