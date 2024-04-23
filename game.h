#pragma once

class Game
{
public:
	Game(int screenWidth, int screenHeight);
	~Game();

	void Run();

private:
	void HandleInput();
	void Update();
	void Render();
};