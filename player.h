#pragma once

#include "entity.h"

enum class PlayerState
{
	IDLE,
	MOVE_RIGHT,
	MOVE_LEFT,
	FIRE,
	DIE
};

struct PlayerV2 : EntityV2
{
	PlayerV2(float screenWidth, float screenHeight) : EntityV2(SPRITE_PLAYER, { screenWidth, screenHeight }) {}	

public:
	int screenWidth;
	int screenHeight;

	void SetState(const PlayerState newState)
	{
		state = newState;
	}

	[[nodiscard]] PlayerState GetState() const { return state; }

	void Update()
	{
		EntityV2::UpdateSprite();

		switch (state)
		{
		case PlayerState::IDLE: return;
		case PlayerState::MOVE_RIGHT: MoveRight(); return;
		case PlayerState::MOVE_LEFT: MoveLeft(); return;
		}
	}	

private:	
	PlayerState state = PlayerState::IDLE;

	void MoveLeft()
	{
		if (coord.x - 15 < 0)
		{
			coord.x = 0;
		}
		else
		{
			coord.x -= 15;
		}
	}

	void MoveRight()
	{
		if (coord.x + 15 > (screenWidth - GetW()))
		{			
			coord.x = screenWidth - GetW();
		}
		else
		{
			coord.x += 15.0f;
		}
	}
};