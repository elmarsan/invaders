#pragma once

#include "entity.h"

enum class InvaderState
{
	IDLE,
	MOVE_DOWN,
	MOVE_RIGHT,
	MOVE_LEFT,
	// FIRE,
	DIE
};

struct Invader : EntityV2
{
	Invader(int type, Point2D coord) : EntityV2(type, coord), state(InvaderState::MOVE_RIGHT) {}
	Invader() : EntityV2(SPRITE_ENEMY_0, { -1, -1 }), state(InvaderState::MOVE_RIGHT) {}

	[[nodiscard]] int GetScore() const
	{
		switch (EntityV2::type)
		{
		case SPRITE_ENEMY_0: return 30;
		case SPRITE_ENEMY_1: return 20;
		case SPRITE_ENEMY_2: return 10;
		case SPRITE_ENEMY_UFO: return 300;
		}
	}

	void SwitchDirection()
	{
		coord.y += 30;
		if (state == InvaderState::MOVE_RIGHT)
		{
			state = InvaderState::MOVE_LEFT;
		}
		else if (state == InvaderState::MOVE_LEFT)
		{
			state = InvaderState::MOVE_RIGHT;
		}
	}

	[[nodiscard]] InvaderState GetState() const { return state; }

	void Update()
	{
		if (!IsActive())
		{			
			return;
		}

		auto ticks = platform::getTicks();
		if (ticks - moveLastTick > moveDelay)
		{
			EntityV2::UpdateSprite();

			switch (state)
			{
			case InvaderState::IDLE: break;
			case InvaderState::MOVE_RIGHT: coord.x += 15; break;
			case InvaderState::MOVE_LEFT: coord.x -= 15; break;
			case InvaderState::MOVE_DOWN: coord.y += 30; break;
			}

			moveLastTick = ticks;
		}
	}	
private:
	InvaderState state = InvaderState::IDLE;	

	int moveDelay = 500;
	int moveLastTick;
};