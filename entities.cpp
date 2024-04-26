#include "entities.h"
#include "platform.h"
#include "audio.h"

Entity::Entity(int type, Point2D coord) : type(type), coord(coord)
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
	case SPRITE_OBSTACLE:
		spriteFrames = spriteObstacle;
		spriteSize = { spriteObstacleWidth, spriteObstacleHeight };
		break;
	}
}

void Entity::Render() const
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

void Entity::UpdateSprite()
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

void Entity::SetSprite(const int spriteId)
{
	switch (spriteId)
	{
	case SPRITE_DESTROY_ENEMY:
		spriteFrames = spriteDestroyEnemy;
		spriteSize = { spriteShipWidth, spriteShipHeight };
		spriteFrameDelay = 300;
		break;
	case SPRITE_DESTROY_PLAYER:
		spriteFrames = spritePlayerDestroy;
		spriteSize = { spriteShipWidth, spriteShipHeight };
		spriteFrameDelay = 100;
		break;
	case SPRITE_PLAYER:
		spriteFrames = spritePlayer;
		spriteSize = { spriteShipWidth, spriteShipHeight };
		spriteFrameDelay = 300;
		break;
	}

	spriteCurrentFrameIdx = 0;
	spriteLastTick = 0;
}

void Entity::Destroy()
{
	coord.x = -1;
	coord.y = -1;
}

void Entity::SetPos(Point2D newCoord)
{
	coord.x = newCoord.x;
	coord.y = newCoord.y;
}

Invader::Invader(int type, Point2D coord) : Entity(type, coord), state(ShipState::MOVE_RIGHT)
{
	switch (Entity::type)
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

Invader::Invader() : Entity(SPRITE_ENEMY_0, { -1, -1 }), state(ShipState::MOVE_RIGHT) {}

void Invader::SetState(const ShipState newState)
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

void Invader::SwitchDirection()
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

void Invader::Update()
{
	if (!IsActive())
	{
		return;
	}

	if (Entity::GetType() == SPRITE_ENEMY_UFO && IsActive())
	{
		platform::playSound(SOUND_UFO, 1);
		coord.x += speedX;
		if (coord.x > screenWidth)
		{
			platform::stopChannel(1);
			Destroy();
		}

		return;
	}

	auto ticks = platform::getTicks();
	if (ticks - moveLastTick > moveDelay)
	{
		Entity::UpdateSprite();

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

Projectile::Projectile() : Entity(SPRITE_PROJECTILE_TYPE_0, { -1, -1 }) {}

Projectile::Projectile(int projectileType, Point2D coord, float screenHeight) : Entity(projectileType, coord), screenHeight(screenHeight)
{
	switch (projectileType)
	{
	case SPRITE_PROJECTILE_TYPE_0:
	case SPRITE_PROJECTILE_TYPE_1:
	case SPRITE_PROJECTILE_TYPE_2:
		state = ProjectileState::MOVE_DOWN;
		break;
	case SPRITE_PROJECTILE_TYPE_PLAYER:
		state = ProjectileState::MOVE_UP;
		break;
	}
}

void Projectile::Update()
{
	if (!Entity::IsActive())
	{
		return;
	}
	else if (screenHeight > 0 && (coord.y >= screenHeight) || (coord.y < 0))
	{
		Entity::Destroy();
	}

	Entity::UpdateSprite();

	switch (state)
	{
	case ProjectileState::IDLE: return;
	case ProjectileState::MOVE_DOWN: coord.y += 5.0f;  return;
	case ProjectileState::MOVE_UP: coord.y -= 8.5f;  return;
	}
}

Player::Player(float screenWidth, float screenHeight) : Entity(SPRITE_PLAYER, { screenWidth, screenHeight }) {}

void Player::SetState(const ShipState newState)
{
	auto ticks = platform::getTicks();

	if (newState == ShipState::DESTROYING)
	{
		destroyLastTick = ticks;
		platform::stopAllSounds();
		platform::playSound(SOUND_PLAYER_EXPLOSION, 1);
		SetSprite(SPRITE_DESTROY_PLAYER);
	}
	else if (newState == ShipState::DESTROYED && state == ShipState::DESTROYING)
	{
		destroyLastTick = 0;
		SetSprite(SPRITE_PLAYER);
	}

	state = newState;
}

void Player::Update()
{
	Entity::UpdateSprite();

	auto ticks = platform::getTicks();
	switch (state)
	{
	case ShipState::IDLE: break;
	case ShipState::MOVE_RIGHT:
	{
		if (coord.x + 15.0f > (screenWidth - GetW()))
		{
			coord.x = screenWidth - GetW();
		}
		else
		{
			coord.x += 15.0f;
		}
		break;
	}
	case ShipState::MOVE_LEFT:
	{
		if (coord.x - 15.0f < 0)
		{
			coord.x = 0;
		}
		else
		{
			coord.x -= 15.0f;
		}
		break;
	}
	case ShipState::DESTROYING:
		if (ticks - destroyLastTick > destroyTicks)
		{
			SetState(ShipState::DESTROYED);
		}
		break;
	}
}

Obstacle::Obstacle() : Entity(SPRITE_OBSTACLE, { -1, -1 }) {}
Obstacle::Obstacle(Point2D coord) : Entity(SPRITE_OBSTACLE, coord) {}

void Obstacle::ReceiveProjectile(const Point2D impactCoord)
{
	life -= 20;
	impactCoords.emplace_back(impactCoord);
	if (life == 0)
	{
		Destroy();
	}
}

void Obstacle::Render()
{
	if (!IsActive())
	{
		return;
	}

	auto texture = GetSprite();
	Rect pos{ GetX(), GetY(), GetW(), GetH() };
	platform::addBuffTexture(texture, pos);	

	for (const auto& i : impactCoords)
	{
		// Vertical impacts
		platform::addBuffRect({ i.x, i.y, 10, 10 }, 0, true);
		platform::addBuffRect({ i.x, i.y + 10, 10, 10 }, 0, true);
		// Horizontal impacts	
		platform::addBuffRect({ i.x + 10, i.y, 10, 10 }, 0, true);
		platform::addBuffRect({ i.x - 10, i.y, 10, 10 }, 0, true);
	}
}