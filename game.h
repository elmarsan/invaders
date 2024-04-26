#pragma once

#include "entities.h"
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
	void RenderPauseMenu();
	void RenderMenu();
	void CheckCollisions();
	void Init();

	[[nodiscard]] bool HasToSwitchInvaderDir();

	void InvaderFire(const int idx, const int projIdx);
	void DestroyInvader(const int idx);

private:
	Player player{ 0, 0 };
	std::array<Projectile, 3> projectiles;
	std::array<Invader, 56> invaders;
	std::array<Obstacle, 4> obstacles{};	
	int ufoIdx = 55;
	bool ufoAppeared = false;
	std::unordered_map<int, bool> invaderCanShoot;
	State state = State::MENU;	
	int screenWidth = 0;
	int screenHeight = 0;
	int score = 0;
	int lifes = 3;

	// Flags for controlling keyboard input.
	// Following keys have to be release in order to trigger their event again.
	bool enableSpaceKey = true;
	bool enableMKey = true;
	bool enableEscapeKey = true;
};