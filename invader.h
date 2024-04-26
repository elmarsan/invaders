#pragma once

#include "entity.h"

struct Invader : EntityV2
{
public:
	int screenWidth = 0;

	Invader(int type, Point2D coord) : EntityV2(type, coord), state(ShipState::MOVE_RIGHT) 
	{
		switch (EntityV2::type)
		{
		case SPRITE_ENEMY_0: 
			score = 30.0f;
			break;
		case SPRITE_ENEMY_1: 
			score = 20.0f;
			break;
		case SPRITE_ENEMY_2: 
			score = 10.0f;
			break;
		case SPRITE_ENEMY_UFO: 
			score = 300; 
			speedX = 2.8f;
			speedY = 0;			
			break;
		}
	}
	Invader() : EntityV2(SPRITE_ENEMY_0, { -1, -1 }), state(ShipState::MOVE_RIGHT) {}

	[[nodiscard]] int GetScore() const { return score; }

	void SetState(const ShipState newState)
	{		
		auto ticks = platform::getTicks();

		if (newState == ShipState::DESTROYING)
		{
			destroyLastTick = ticks;
			SetSprite(SPRITE_DESTROY_ENEMY);
		}
		else if (newState == ShipState::DESTROYED && state == ShipState::DESTROYING)
		{
			Destroy();
		}

		state = newState;
	}

	void SwitchDirection()
	{
		coord.y += 30;
		if (state == ShipState::MOVE_RIGHT)
		{
			state = ShipState::MOVE_LEFT;
		}
		else if (state == ShipState::MOVE_LEFT)
		{
			state = ShipState::MOVE_RIGHT;
		}
	}

	[[nodiscard]] ShipState GetState() const { return state; }

	void Update()
	{
		if (!IsActive())
		{			
			return;
		}

		if (EntityV2::GetType() == SPRITE_ENEMY_UFO)
		{
			coord.x += speedX;

			/*if (coord.x >= screenWidth)
			{
				Destroy();
			}*/

			return;
		}

		auto ticks = platform::getTicks();
		if (ticks - moveLastTick > moveDelay)
		{
			EntityV2::UpdateSprite();

			switch (state)
			{
			case ShipState::IDLE: break;
			case ShipState::MOVE_RIGHT: coord.x += speedX; break;
			case ShipState::MOVE_LEFT: coord.x -= speedX; break;
			case ShipState::MOVE_DOWN: coord.y += speedY; break;
			case ShipState::DESTROYING:
				if (ticks - destroyLastTick > destroyTicks)
				{
					SetState(ShipState::DESTROYED);
				}
				break;
			}

			moveLastTick = ticks;
		}
	}	
private:
	ShipState state = ShipState::IDLE;
	int moveDelay = 500;
	int moveLastTick = 0;
	int destroyTicks = 50;
	int destroyLastTick = 0;
	float speedX = 15.0f;
	float speedY = 30.0f;
	int score = 0;
};