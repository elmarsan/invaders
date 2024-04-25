#pragma once
#include "asset.h"
#include "platform.h"

// TODO: move to asset.h
enum
{
	ANIMATION_INVADER_DESTROY,
	ANIMATION_PLAYER_DESTROY,
};

struct EntityV2
{
	EntityV2(int type, Point2D coord) : type(type), coord(coord)
	{		
		switch (type)
		{
		case SPRITE_ENEMY_0:
			spriteFrames = spriteEnemy0;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			break;
		case SPRITE_ENEMY_1:
			spriteFrames = spriteEnemy1;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			break;
		case SPRITE_ENEMY_2:
			spriteFrames = spriteEnemy2;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			break;
		case SPRITE_ENEMY_UFO:
			spriteFrames = spriteEnemyUfo;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			break;
		case SPRITE_PLAYER:
			spriteFrames = spritePlayer;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			break;
		case SPRITE_PROJECTILE_TYPE_0:			
			spriteFrames = spriteProjectile0;
			spriteSize = { spriteProjectileWidth, spriteProjectileHeight };
			break;
		case SPRITE_PROJECTILE_TYPE_1:
			spriteFrames = spriteProjectile1;
			spriteSize = { spriteProjectileWidth, spriteProjectileHeight };
			break;
		case SPRITE_PROJECTILE_TYPE_2:
			spriteFrames = spriteProjectile2;
			spriteSize = { spriteProjectileWidth, spriteProjectileHeight };
			break;
		case SPRITE_PROJECTILE_TYPE_PLAYER:
			spriteFrames = spriteProjectilePlayer;
			spriteSize = { spriteProjectileWidth, spriteProjectileHeight };
			break;
		}
	}

	void Render() const
	{
		if (!IsActive())
		{
			return;
		}

		Rect dst{ GetX(), GetY(), GetW(), GetH() };
		float x = spriteFrames[spriteCurrentFrameIdx].x;
		float y = spriteFrames[spriteCurrentFrameIdx].y;
		Rect src{ x, y, spriteSize.w, spriteSize.h };
		platform::addBuffTexture(src, dst);
	}

	void UpdateSprite()
	{		
		if (spriteFrames.size() <= 1)
		{
			return;
		}

		auto ticks = platform::getTicks();
		if (ticks - spriteLastTick > spriteFrameDelay)
		{
			if (++spriteCurrentFrameIdx >= spriteFrames.size())
			{
				spriteCurrentFrameIdx = 0;
			}
			spriteLastTick = ticks;
		}
	}

	void SetSprite(const int animationId)
	{
		switch (animationId)
		{
		case ANIMATION_INVADER_DESTROY:
			spriteFrames = spriteDestroyEnemy;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			spriteFrameDelay = 300;
			break;
		case ANIMATION_PLAYER_DESTROY:
			spriteFrames = spritePlayerDestroy;
			spriteSize = { spriteShipWidth, spriteShipHeight };
			spriteFrameDelay = 300;
			break;
		}

		spriteCurrentFrameIdx = 0;
		spriteLastTick = 0;
	}

	[[nodiscard]] bool IsActive() const { return coord.x != -1 && coord.y != -1; }
	void Destroy()
	{	
		coord.x = -1;
		coord.y = -1;
	}

	[[nodiscard]] float GetX() const { return coord.x; }
	[[nodiscard]] float GetY() const { return coord.y; }
	[[nodiscard]] float GetW() const { return spriteSize.w * spriteScaleX; }
	[[nodiscard]] float GetH() const { return spriteSize.h * spriteScaleY; }

	void SetPos(Point2D newCoord)
	{
		coord.x = newCoord.x;
		coord.y = newCoord.y;
	}

	[[nodiscard]] Rect GetCollider() const
	{
		return Rect{ coord.x, coord.y, GetW(), GetH() };
	}

	[[nodiscard]] int GetType() const { return type; }

private:	
	SpriteFrames spriteFrames;
	Size2D spriteSize;
	int spriteCurrentFrameIdx = 0;
	int spriteFrameDelay = 0;
	int spriteLastTick = 0;	

protected:
	int type;
	Point2D coord;
};