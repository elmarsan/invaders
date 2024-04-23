#pragma once

#include <vector>
#include <array>
#include "util.h"

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

enum FontCharacter
{
	FONT_CHARACTER_A,
	FONT_CHARACTER_B,
	FONT_CHARACTER_C,
	FONT_CHARACTER_D,
	FONT_CHARACTER_E,
	FONT_CHARACTER_F,
	FONT_CHARACTER_G,
	FONT_CHARACTER_H,
	FONT_CHARACTER_I,
	FONT_CHARACTER_J,
	FONT_CHARACTER_K,
	FONT_CHARACTER_L,
	FONT_CHARACTER_M,
	FONT_CHARACTER_N,
	FONT_CHARACTER_O,
	FONT_CHARACTER_P,
	FONT_CHARACTER_Q,
	FONT_CHARACTER_R,
	FONT_CHARACTER_S,
	FONT_CHARACTER_T,
	FONT_CHARACTER_U,
	FONT_CHARACTER_V,
	FONT_CHARACTER_W,
	FONT_CHARACTER_X,
	FONT_CHARACTER_Y,
	FONT_CHARACTER_Z,
	// Numbers
	FONT_CHARACTER_0,
	FONT_CHARACTER_1,
	FONT_CHARACTER_2,
	FONT_CHARACTER_3,
	FONT_CHARACTER_4,
	FONT_CHARACTER_5,
	FONT_CHARACTER_6,
	FONT_CHARACTER_7,
	FONT_CHARACTER_8,
	FONT_CHARACTER_9,
	// Special
	FONT_CHARACTER_EQUAL,
	FONT_CHARACTER_ASTERISK,

	FONT_CHARACTER_COUNT,
};

const int spriteScaleX = 3;
const int spriteScaleY = 3;

const int spriteFontWidth = 8;
const int spriteFontHeight = 8;

const int spriteShipWidth = 16;
const int spriteShipHeight = 8;

const int spriteObstacleWidth = 24;
const int spriteObstacleHeight = 16;

const int spriteProjectileWidth = 3;
const int spriteProjectileHeight = 8;

using SpriteFrames = std::vector<Point2D>;

const SpriteFrames spriteEnemy0{
	{1, 1},
	{1, 11}
};

const SpriteFrames spriteEnemy1{
	{19, 1},
	{19, 11}
};

const SpriteFrames spriteEnemy2{
	{37, 1},
	{37, 11}
};

const SpriteFrames spriteEnemyUfo{
	{1, 39}
};

const SpriteFrames spritePlayer{
	{1, 49},
};

const SpriteFrames spriteProjectile0{
	{1, 21},
	{6, 21},
	{11, 21},
	{16, 21},
};

const SpriteFrames spriteProjectile1{
	{21, 21},
	{26, 21},
	{31, 21},
	{36, 21},
};

const SpriteFrames spriteProjectile2{
	{41, 21},
	{46, 21},
	{51, 21},
	{56, 21},
};

const SpriteFrames spriteProjectilePlayer{
	{41, 21},
};

//const std::vector<Rect> spriteNumbers{
//	Rect{21, 99, 8, 8}, // 0
//	Rect{31, 99, 8, 8}, // 1
//	Rect{41, 99, 8, 8}, // 2
//	Rect{51, 99, 8, 8}, // 3
//	Rect{61, 99, 8, 8}, // 4
//	Rect{71, 99, 8, 8}, // 5
//	Rect{1, 109, 8, 8}, // 6
//	Rect{11, 109, 8, 8}, // 7
//	Rect{21, 109, 8, 8}, // 8
//	Rect{31, 109, 8, 8}, // 9
//};

//enum SpriteLetter
//{
//	SPRITE_LETTER_A,
//	SPRITE_LETTER_B,
//	SPRITE_LETTER_C,
//	SPRITE_LETTER_D,
//	SPRITE_LETTER_E,
//	SPRITE_LETTER_F,
//	SPRITE_LETTER_G,
//	SPRITE_LETTER_H,
//	SPRITE_LETTER_I,
//	SPRITE_LETTER_J,
//	SPRITE_LETTER_K,
//	SPRITE_LETTER_L,
//	SPRITE_LETTER_M,
//	SPRITE_LETTER_N,
//	SPRITE_LETTER_O,
//	SPRITE_LETTER_P,
//	SPRITE_LETTER_Q,
//	SPRITE_LETTER_R,
//	SPRITE_LETTER_S,
//	SPRITE_LETTER_T,
//	SPRITE_LETTER_U,
//	SPRITE_LETTER_V,
//	SPRITE_LETTER_W,
//	SPRITE_LETTER_X,
//	SPRITE_LETTER_Y,
//	SPRITE_LETTER_Z,
//
//	SPRITE_LETTER_COUNT,
//};
//
//const std::array<Rect, SPRITE_LETTER_COUNT> spriteLetters{
//	Rect{1, 69, 8, 8}, // A
//	Rect{11, 69, 8, 8}, // B
//	Rect{21, 69, 8, 8}, // C
//	Rect{31, 69, 8, 8}, // D
//	Rect{41, 69, 8, 8}, // E
//	Rect{51, 69, 8, 8}, // F
//	Rect{61, 69, 8, 8}, // G
//	Rect{71, 69, 8, 8}, // H	
//	Rect{1, 79, 8, 8}, // I
//	Rect{11, 79, 8, 8}, // J
//	Rect{21, 79, 8, 8}, // K
//	Rect{31, 79, 8, 8}, // L
//	Rect{41, 79, 8, 8}, // M
//	Rect{51, 79, 8, 8}, // N
//	Rect{61, 79, 8, 8}, // O
//	Rect{71, 79, 8, 8}, // P
//	Rect{1, 89, 8, 8}, // Q
//	Rect{11, 89, 8, 8}, // R
//	Rect{21, 89, 8, 8}, // S
//	Rect{31, 89, 8, 8}, // T
//	Rect{41, 89, 8, 8}, // U
//	Rect{51, 89, 8, 8}, // V
//	Rect{61, 89, 8, 8}, // W
//	Rect{71, 89, 8, 8}, // X
//	Rect{1, 99, 8, 8}, // X
//	Rect{11, 99, 8, 8}, // Z
//};

const SpriteFrames spriteObstacle{
	{45, 31},
};

const SpriteFrames spriteDestroyEnemy{
	{55, 1},
};

const std::vector<Point2D> spriteFonts{
	{1, 69},	// A
	{11, 69},	// B
	{21, 69},	// C
	{31, 69},	// D
	{41, 69},	// E
	{51, 69},	// F
	{61, 69},	// G
	{71, 69},	// H	
	{1, 79},	// I
	{11, 79},	// J
	{21, 79},	// K
	{31, 79},	// L
	{41, 79},	// M
	{51, 79},	// N
	{61, 79},	// O
	{71, 79},	// P
	{1, 89},	// Q
	{11, 89},	// R
	{21, 89},	// S
	{31, 89},	// T
	{41, 89},	// U
	{51, 89},	// V
	{61, 89},	// W
	{71, 89},	// X
	{1, 99},	// Y
	{11, 99},	// Z
	{21, 99},	// 0
	{31, 99},	// 1
	{41, 99},	// 2
	{51, 99},	// 3
	{61, 99},	// 4
	{71, 99},	// 5
	{1, 109},	// 6
	{11, 109},	// 7
	{21, 109},	// 8
	{31, 109},	// 9
	{61, 109},	// =
	{71, 109},	// *
};