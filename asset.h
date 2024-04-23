#pragma once

#include "rect.h"
#include <vector>
#include <array>

const std::vector<Rect> spriteEnemy0{
	Rect{1, 1, 16, 8},
	Rect{1, 11, 16, 8}
};

const std::vector<Rect> spriteEnemy1{
	Rect{19, 1, 16, 8},
	Rect{19, 11, 16, 8}
};

const std::vector<Rect> spriteEnemy2{
	Rect{37, 1, 16, 8},
	Rect{37, 11, 16, 8}
};

const std::vector<Rect> spritePlayer{
	Rect{1, 49, 16, 8},
};

const std::vector<Rect> spriteProjectile0{
	Rect{1, 21, 3, 8},
	Rect{6, 21, 3, 8},
	Rect{11, 21, 3, 8},
	Rect{16, 21, 3, 8},
};

const std::vector<Rect> spriteProjectile1{
	Rect{21, 21, 3, 8},
	Rect{26, 21, 3, 8},
	Rect{31, 21, 3, 8},
	Rect{36, 21, 3, 8},
};

const std::vector<Rect> spriteProjectile2{
	Rect{41, 21, 3, 8},
	Rect{46, 21, 3, 8},
	Rect{51, 21, 3, 8},
	Rect{56, 21, 3, 8},
};

const std::vector<Rect> spriteProjectilePlayer{
	Rect{41, 21, 3, 8},
};

const std::vector<Rect> spriteNumbers{
	Rect{21, 99, 8, 8}, // 0
	Rect{31, 99, 8, 8}, // 1
	Rect{41, 99, 8, 8}, // 2
	Rect{51, 99, 8, 8}, // 3
	Rect{61, 99, 8, 8}, // 4
	Rect{71, 99, 8, 8}, // 5
	Rect{1, 109, 8, 8}, // 6
	Rect{11, 109, 8, 8}, // 7
	Rect{21, 109, 8, 8}, // 8
	Rect{31, 109, 8, 8}, // 9
};

enum SpriteLetter
{
	SPRITE_LETTER_A,
	SPRITE_LETTER_B,
	SPRITE_LETTER_C,
	SPRITE_LETTER_D,
	SPRITE_LETTER_E,
	SPRITE_LETTER_F,
	SPRITE_LETTER_G,
	SPRITE_LETTER_H,
	SPRITE_LETTER_I,
	SPRITE_LETTER_J,
	SPRITE_LETTER_K,
	SPRITE_LETTER_L,
	SPRITE_LETTER_M,
	SPRITE_LETTER_N,
	SPRITE_LETTER_O,
	SPRITE_LETTER_P,
	SPRITE_LETTER_Q,
	SPRITE_LETTER_R,
	SPRITE_LETTER_S,
	SPRITE_LETTER_T,
	SPRITE_LETTER_U,
	SPRITE_LETTER_V,
	SPRITE_LETTER_W,
	SPRITE_LETTER_X,
	SPRITE_LETTER_Y,
	SPRITE_LETTER_Z,

	SPRITE_LETTER_COUNT,
};

const std::array<Rect, SPRITE_LETTER_COUNT> spriteLetters{
	Rect{1, 69, 8, 8}, // A
	Rect{11, 69, 8, 8}, // B
	Rect{21, 69, 8, 8}, // C
	Rect{31, 69, 8, 8}, // D
	Rect{41, 69, 8, 8}, // E
	Rect{51, 69, 8, 8}, // F
	Rect{61, 69, 8, 8}, // G
	Rect{71, 69, 8, 8}, // H	
	Rect{1, 79, 8, 8}, // I
	Rect{11, 79, 8, 8}, // J
	Rect{21, 79, 8, 8}, // K
	Rect{31, 79, 8, 8}, // L
	Rect{41, 79, 8, 8}, // M
	Rect{51, 79, 8, 8}, // N
	Rect{61, 79, 8, 8}, // O
	Rect{71, 79, 8, 8}, // P
	Rect{1, 89, 8, 8}, // Q
	Rect{11, 89, 8, 8}, // R
	Rect{21, 89, 8, 8}, // S
	Rect{31, 89, 8, 8}, // T
	Rect{41, 89, 8, 8}, // U
	Rect{51, 89, 8, 8}, // V
	Rect{61, 89, 8, 8}, // W
	Rect{71, 89, 8, 8}, // X
	Rect{1, 99, 8, 8}, // X
	Rect{11, 99, 8, 8}, // Z
};

const std::vector<Rect> spriteObstacle{
	Rect{45, 31, 24, 16},
};

const std::vector<Rect> spriteDestroyEnemy{
	Rect{55, 1, 16, 8},
};

enum SpriteType
{
	SPRITE_ENEMY_0,
	SPRITE_ENEMY_1,
	SPRITE_ENEMY_2,
	SPRITE_PLAYER,
	SPRITE_PROJECTILE_TYPE_0,
	SPRITE_PROJECTILE_TYPE_1,
	SPRITE_PROJECTILE_TYPE_2,
	SPRITE_PROJECTILE_TYPE_PLAYER,
	SPRITE_OBSTACLE,
	SPRITE_DESTROY_ENEMY,
};

const int spriteScaleX = 3.5;
const int spriteScaleY = 3.5;

const int spriteFontScaleX = 3;
const int spriteFontScaleY = 3;

const int spriteFontW = 8;
const int spriteFontY = 8;