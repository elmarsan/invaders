#pragma once

#include "util.h"
#include "asset.h"

struct Entity
{
	Entity(int type, Point2D coord);

	void Render() const;
	void UpdateSprite();
	void SetSprite(const int spriteId);

	[[nodiscard]] bool IsActive() const { return coord.x != -1 && coord.y != -1; }
	[[nodiscard]] int GetType() const { return type; }

	[[nodiscard]] float GetX() const { return coord.x; }
	[[nodiscard]] float GetY() const { return coord.y; }
	[[nodiscard]] float GetW() const { return spriteSize.w * spriteScaleX; }
	[[nodiscard]] float GetH() const { return spriteSize.h * spriteScaleY; }

	[[nodiscard]] Rect GetCollider() const
	{
		return Rect{ coord.x, coord.y, GetW(), GetH() };
	}

	[[nodiscard]] Rect GetSprite() const
	{
		float x = spriteFrames[spriteCurrentFrameIdx].x;
		float y = spriteFrames[spriteCurrentFrameIdx].y;
		return Rect{ x, y, spriteSize.w, spriteSize.h };
	}

	void Destroy();
	void SetPos(Point2D newCoord);

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

enum class ShipState
{
	IDLE,
	MOVE_DOWN,
	MOVE_RIGHT,
	MOVE_LEFT,
	DESTROYING,
	DESTROYED
};

struct Invader : Entity
{
public:
	int screenWidth = 0;

	Invader();
	Invader(int type, Point2D coord);

	[[nodiscard]] int GetScore() const { return score; }
	[[nodiscard]] ShipState GetState() const { return state; }

	void SetState(const ShipState newState);
	void SwitchDirection();
	void Update();

private:
	ShipState state = ShipState::IDLE;
	int moveDelay = 300;
	int moveLastTick = 0;
	int destroyTicks = 50;
	int destroyLastTick = 0;
	float speedX = 15.0f;
	float speedY = 30.0f;
	int score = 0;
};

enum class ProjectileState
{
	IDLE,
	MOVE_DOWN,
	MOVE_UP
};

struct Projectile : Entity
{
public:
	int screenHeight;

	Projectile();
	Projectile(int projectileType, Point2D coord, float screenHeight);

	void Update();

private:
	ProjectileState state = ProjectileState::IDLE;
};

struct Player : Entity
{
	Player(float screenWidth, float screenHeight);

	void SetState(const ShipState newState);
	void Update();

	[[nodiscard]] ShipState GetState() const { return state; }

public:
	int screenWidth;
	int screenHeight;

private:
	ShipState state = ShipState::IDLE;

	int destroyTicks = 1200;
	int destroyLastTick = 0;
};

struct Obstacle : Entity
{
	Obstacle();
	Obstacle(Point2D coord);

	void ReceiveProjectile(const Point2D impactCoord);
	void Render();
private:
	int life = 180;
	std::vector<Point2D> impactCoords;
};