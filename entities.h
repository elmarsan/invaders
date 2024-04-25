#pragma once

#include <vector>
#include <iostream>
#include "platform.h"
#include "util.h"
#include "asset.h"

const float enemyProjectileSpeed = 4.5f;
const float playerProjectileSpeed = 7.5f;

struct Sprite
{
	Sprite() {}
	Sprite(const SpriteFrames& frames, float w, float h) : frames(frames), w(w), h(h)
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

	void SetSprite(const SpriteFrames& newFrames, float newW, float newH)
	{
		frames = newFrames;
		w = newW;
		h = newH;
		NextFrame();
	}

	[[nodiscard]] Rect GetTexture() const
	{
		float x = frames[currentFrameIdx].x;
		float y = frames[currentFrameIdx].y;
		return Rect{ x, y, w, h };
	}

private:
	SpriteFrames frames = {};
	float w;
	float h;
	int currentFrameIdx = 0;
};

struct Entity
{
	Entity(int spriteType, float x, float y) : x(x), y(y)
	{
		switch (spriteType)
		{
		case SPRITE_ENEMY_0: sprite = { spriteEnemy0, spriteShipWidth, spriteShipHeight }; break;
		case SPRITE_ENEMY_1: sprite = { spriteEnemy1, spriteShipWidth, spriteShipHeight }; break;
		case SPRITE_ENEMY_2: sprite = { spriteEnemy2, spriteShipWidth, spriteShipHeight }; break;
		case SPRITE_ENEMY_UFO: sprite = { spriteEnemyUfo, spriteShipWidth, spriteShipHeight }; break;
		case SPRITE_PLAYER: sprite = { spritePlayer, spriteShipWidth, spriteShipHeight }; break;
		case SPRITE_PROJECTILE_TYPE_0: sprite = { spriteProjectile0, spriteProjectileWidth, spriteProjectileHeight }; break;
		case SPRITE_PROJECTILE_TYPE_1: sprite = { spriteProjectile1, spriteProjectileWidth, spriteProjectileHeight }; break;
		case SPRITE_PROJECTILE_TYPE_2: sprite = { spriteProjectile2, spriteProjectileWidth, spriteProjectileHeight }; break;
		case SPRITE_PROJECTILE_TYPE_PLAYER: sprite = { spriteProjectilePlayer, spriteProjectileWidth, spriteProjectileHeight }; break;
		case SPRITE_OBSTACLE: sprite = { spriteObstacle, spriteObstacleWidth, spriteObstacleHeight }; break;
		case SPRITE_DESTROY_ENEMY: sprite = { spriteDestroyEnemy, spriteShipWidth, spriteShipHeight }; break;
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

	void SetSprite(const SpriteFrames& newFrames, float newW, float newH)
	{
		sprite.SetSprite(newFrames, newW, newH);
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
		case SPRITE_ENEMY_UFO:
			score = 300;
			projectileType = -1;
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

	void MoveRight2()
	{
		x += 2.1f;
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
		SetSprite(spriteDestroyEnemy, spriteShipWidth, spriteShipHeight);
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
			y -= playerProjectileSpeed;
		}
		else
		{
			y += enemyProjectileSpeed;
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

	void Die()
	{
		if (death)
		{
			return;
		}

		isDying = true;
		dieStartMili = platform::getTicks();
		SetSprite(spritePlayerDestroy, spriteShipWidth, spriteShipHeight);
	}

	[[nodiscard]] bool IsDying() const { return isDying; }

	void Update()
	{
		if (death)
		{
			death = false;
			x = (800.0f / 2);
			y = (640.0f - 100);
			sprite.SetSprite(spritePlayer, spriteShipWidth, spriteShipHeight);
		}

		if (isDying)
		{
			auto ticks = platform::getTicks();
			if (dieLastFrame + ticks >= dieFrameSpeed)
			{
				sprite.NextFrame();
				dieLastFrame = ticks;
			}

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
	bool isDying = false;
	bool death = false;
	uint64_t dieStartMili = 0;

	int dieFrameSpeed = 250;
	int dieLastFrame = 0;
};

struct Obstacle : Entity
{
	Obstacle() : Obstacle(-1, -1) {}
	Obstacle(float x, float y) : Entity(SPRITE_OBSTACLE, x, y) {}

	void ReceiveProjectile(const Point2D impactCoord)
	{
		std::cout << "Impact" << std::endl;
		life -= 20;
		impactCoords.emplace_back(impactCoord);
		if (life == 0)
		{
			Destroy();
		}
	}

	Rect GetCollider() const
	{
		auto c = Entity::GetCollider();
		if (collider.x >= 0 && collider.y >= 0)
		{
			c.x = collider.x;
			c.y = collider.y;
		}
		return c;
	}

	void Render()
	{
		if (!Entity::IsAlive())
		{
			return;
		}

		auto texture = Entity::GetTexture();
		Rect pos{ Entity::GetPosX(), Entity::GetPosY(), Entity::GetW(), Entity::GetH() };
		platform::addBuffTexture(texture, pos);
		platform::addBuffRect(Entity::GetCollider(), 0xff000000, false);

		uint32_t white = 0xffffffff;
		uint32_t black = 0;
		uint32_t color = black;

		for (const auto& i : impactCoords)
		{
			// Vertical 		
			platform::addBuffRect({ i.x, i.y, 10, 10 }, color, true);
			platform::addBuffRect({ i.x, i.y + 10, 10, 10 }, color, true);

			// Horizontal 			
			platform::addBuffRect({ i.x + 10, i.y, 10, 10 }, color, true);
			platform::addBuffRect({ i.x - 10, i.y, 10, 10 }, color, true);

			if (x + 10 >= collider.x)
			{
				collider.x = x + 10;
			}

			if (y + 10 >= collider.y)
			{
				collider.y = y + 10;
			}
		}
	}
private:
	int life = 100;
	std::vector<Point2D> impactCoords;
	Rect collider;
};