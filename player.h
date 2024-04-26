#pragma once

#include "entity.h"

struct PlayerV2 : EntityV2
{
	PlayerV2(float screenWidth, float screenHeight) : EntityV2(SPRITE_PLAYER, { screenWidth, screenHeight }) {}

public:
	int screenWidth;
	int screenHeight;

	void SetState(const ShipState newState)
	{
		auto ticks = platform::getTicks();

		if (newState == ShipState::DESTROYING)
		{			
			destroyLastTick = ticks;
			SetSprite(SPRITE_DESTROY_PLAYER);
		}
		else if (newState == ShipState::DESTROYED && state == ShipState::DESTROYING)
		{
			destroyLastTick = 0;
			SetSprite(SPRITE_PLAYER);
		}

		state = newState;
	}

	[[nodiscard]] ShipState GetState() const { return state; }

	void Update()
	{
		EntityV2::UpdateSprite();

		auto ticks = platform::getTicks();
		switch (state)
		{
		case ShipState::IDLE: break;
		case ShipState::MOVE_RIGHT: MoveRight(); break;
		case ShipState::MOVE_LEFT: MoveLeft(); break;
		case ShipState::DESTROYING:
			if (ticks - destroyLastTick > destroyTicks)
			{
				SetState(ShipState::DESTROYED);
			}
			break;
		}
	}

private:
	ShipState state = ShipState::IDLE;

	int destroyTicks = 1200;
	int destroyLastTick = 0;

	void MoveLeft()
	{
		if (coord.x - 15.0f < 0)
		{
			coord.x = 0;
		}
		else
		{
			coord.x -= 15.0f;
		}
	}

	void MoveRight()
	{
		if (coord.x + 15.0f > (screenWidth - GetW()))
		{
			coord.x = screenWidth - GetW();
		}
		else
		{
			coord.x += 15.0f;
		}
	}
};