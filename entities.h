#pragma once

#include <vector>
#include <iostream>
#include "platform.h"
#include "rect.h"
#include "asset.h"

const float projectileSpeed = 4.5f;

struct Sprite
{
	Sprite() {}
	Sprite(const std::vector<Rect>& frames) : frames(frames)
	{
		NextFrame();
	}	

	void NextFrame()
	{
		if (++currentFrameIdx >= frames.size())
		{
			currentFrameIdx = 0;
		}
	}

	void SetSprite(const std::vector<Rect>& newFrames)
	{
		frames = newFrames;
		NextFrame();
	}

	[[nodiscard]] Rect GetTexture() const
	{
		return frames[currentFrameIdx];
	}

private:
	std::vector<Rect> frames = {};
	int currentFrameIdx = 0;	
};

struct Entity
{
	Entity(int spriteType, float x, float y) : x(x), y(y)
	{
		switch (spriteType)
		{
		case SPRITE_ENEMY_0: sprite = { spriteEnemy0 }; break;


		case SPRITE_ENEMY_1: sprite = { spriteEnemy1 }; break;
		case SPRITE_ENEMY_2: sprite = { spriteEnemy2 }; break;
		case SPRITE_PLAYER: sprite = { spritePlayer }; break;
		case SPRITE_PROJECTILE_TYPE_0: sprite = { spriteProjectile0 }; break;
		case SPRITE_PROJECTILE_TYPE_1: sprite = { spriteProjectile1 }; break;
		case SPRITE_PROJECTILE_TYPE_2: sprite = { spriteProjectile2 }; break;
		case SPRITE_PROJECTILE_TYPE_PLAYER: sprite = { spriteProjectilePlayer }; break;
		case SPRITE_OBSTACLE: sprite = { spriteObstacle }; break;
		case SPRITE_DESTROY_ENEMY: sprite = { spriteDestroyEnemy }; break;
		}
	}

	void Destroy()
	{
		x = -1;
		y = -1;
	}

	[[nodiscard]] float GetPosX() const { return x; }
	[[nodiscard]] float GetPosY() const { return y; }
	[[nodiscard]] float GetW() const { return sprite.GetTexture().w * spriteScaleX; }
	[[nodiscard]] float GetH() const { return sprite.GetTexture().h * spriteScaleY; }

	[[nodiscard]] Rect GetTexture() const { return sprite.GetTexture(); }

	[[nodiscard]] const Rect GetCollider() const
	{
		return Rect{ x, y, GetW(), GetH() };
	}

	[[nodiscard]] bool IsAlive() const { return x != -1 && y != -1; }

	void SetSprite(const std::vector<Rect>& newFrames)
	{
		sprite.SetSprite(newFrames);
	}

protected:
	float x;
	float y;
	Sprite sprite;
};

struct Enemy : Entity
{
	Enemy() : Entity(SPRITE_ENEMY_0, -1, -1) {}
	Enemy(int sprite, float x, float y) : Entity(sprite, x, y)
	{
		switch (sprite)
		{
		case SPRITE_ENEMY_0:
			score = 40;
			projectileType = SPRITE_PROJECTILE_TYPE_0;
			break;
		case SPRITE_ENEMY_1:
			score = 20;
			projectileType = SPRITE_PROJECTILE_TYPE_1;
			break;
		case SPRITE_ENEMY_2:
			score = 10;
			projectileType = SPRITE_PROJECTILE_TYPE_2;
			break;
		}
	}

	void MoveLeft()
	{
		x -= 15;
		sprite.NextFrame();
	}

	void MoveRight()
	{
		x += 15;
		sprite.NextFrame();
	}

	void MoveDown()
	{
		y += 15;
		sprite.NextFrame();
	}

	[[nodiscard]] int GetScore() const { return score; }
	[[nodiscard]] int GetProjectileType() const { return projectileType; }

	void Die()
	{
		if (death)
		{
			return;
		}

		isDying = true;
		dieStartMili = platform::getTicks();
		SetSprite(spriteDestroyEnemy);
	}

	[[nodiscard]] bool IsDying() const { return isDying; }

	void Update()
	{
		if (isDying)
		{
			if (dieStartMili + 350 <= platform::getTicks())
			{
				std::cout << "Died" << std::endl;
				isDying = false;
				death = true;
				Destroy();
			}
		}
	}

private:
	int score = 0;
	int projectileType = 0;

	bool isDying = false;
	bool death = false;
	uint64_t dieStartMili = 0;
};

struct Projectile : Entity
{
	Projectile() : Entity(SPRITE_PROJECTILE_TYPE_PLAYER, -1, -1) {}
	Projectile(int sprite, float x, float y) : Entity(sprite, x, y), type(sprite) {}

	void SetPos(const float newX, const float newY)
	{
		x = newX;
		y = newY;
	}

	void UpdatePos()
	{
		if (type == SPRITE_PROJECTILE_TYPE_PLAYER)
		{
			y -= projectileSpeed;
		}
		else
		{
			y += projectileSpeed;
		}

		sprite.NextFrame();

		if (IsOutOfBound())
		{
			Destroy();
		}
	}

private:
	int type = SPRITE_PROJECTILE_TYPE_PLAYER;

	[[nodiscard]] bool IsOutOfBound() const
	{
		if (type == SPRITE_PROJECTILE_TYPE_PLAYER && y < 0)
		{
			return true;
		}
		// TODO: Fix!!!!!!!!!!!
		else if (y > 800)
		{
			return true;
		}
		return false;
	}
};

struct Player : Entity
{
	// TODO: Fix!!!!!!!!!!!
	Player() : Entity(SPRITE_PLAYER, (800.0f / 2), (640.0f - 100)) {}

	void MoveLeft()
	{
		if (x - 15 < 0)
		{
			x = 0;
		}
		else
		{
			x -= 15;
		}
		sprite.NextFrame();
	}

	void MoveRight()
	{
		// TODO: Fix!!!!!!!!!!!
		if (x + 15 > (800.0f - GetW()))
		{
			// TODO: Fix!!!!!!!!!!!
			x = (800.0f - GetW());
		}
		else
		{
			x += 15.0f;
		}
		sprite.NextFrame();
	}
};

struct Obstacle : Entity
{
	Obstacle() : Obstacle(-1, -1) {}
	Obstacle(float x, float y) : Entity(SPRITE_OBSTACLE, x, y) {}

	void ReceiveProjectile()
	{
		std::cout << "Impact" << std::endl;
		life -= 20;
		if (life == 0)
		{
			Destroy();
		}
	}

private:
	int life = 100;
};
