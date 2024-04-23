#include "game.h"
#include "platform.h"

Game::Game(int screenWidth, int screenHeight) 
{
}

Game::~Game()
{
	platform::exit();
}

void Game::Run()
{
	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	uint64_t frameStart = 0;
	int frameTime;

	while (running)
	{
		frameStart = platform::getTicks();

		HandleInput();
		Update();
		Render();

		do {
			frameTime = platform::getTicks() - frameStart;
		} while (frameDelay > frameTime);
	}
}