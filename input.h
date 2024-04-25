#pragma once

#include <array>

enum KEY_CODE
{
	KEY_CODE_LEFT,
	KEY_CODE_RIGHT,
	KEY_CODE_SPACE,
	KEY_CODE_ESCAPE,
	KEY_CODE_M,

	KEY_CODE_COUNT,
};

struct InputKey
{
	bool isDown;
	bool isUp;
};

[[nodiscard]] bool isKeyDown(int code);
[[nodiscard]] bool isKeyUp(int code);

void keyDown(int code);
void keyUp(int code);
void clearInputKeys();
void clearKey(int code);