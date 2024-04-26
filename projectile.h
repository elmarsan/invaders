#include "entity.h"
#include "asset.h"

enum class ProjectileState
{
	IDLE,
	MOVE_DOWN,
	MOVE_UP
};

struct ProjectileV2 : EntityV2
{
public:	
	int screenHeight;

	ProjectileV2() : EntityV2(SPRITE_PROJECTILE_TYPE_0, { -1, -1 }) {}

	ProjectileV2(int projectileType, Point2D coord, float screenHeight) : EntityV2(projectileType, coord), screenHeight(screenHeight)
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

	void Update()
	{
		if (screenHeight > 0 && coord.y >= screenHeight)
		{
			EntityV2::Destroy();
		}

		if (!EntityV2::IsActive())
		{
			return;
		}

		EntityV2::UpdateSprite();

		switch (state)
		{
		case ProjectileState::IDLE: return;
		case ProjectileState::MOVE_DOWN: coord.y += 5.0f;  return;
		case ProjectileState::MOVE_UP: coord.y -= 8.5f;  return;
		}
	}

private:
	ProjectileState state = ProjectileState::IDLE;
};