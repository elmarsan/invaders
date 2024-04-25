#include "input.h"

std::array<InputKey, KEY_CODE_COUNT> inputKeys;

bool isKeyDown(int code) { return inputKeys[code].isDown; }
bool isKeyUp(int code) { return inputKeys[code].isUp; }

void keyDown(int code)
{
	inputKeys[code].isDown = true;
	inputKeys[code].isUp = false;
}

void keyUp(int code)
{
	inputKeys[code].isDown = false;
	inputKeys[code].isUp = true;
}

void clearInputKeys()
{
	for (auto& key : inputKeys)
	{
		key.isDown = false;
		key.isUp = false;
	}
}

void clearKey(int code)
{
	inputKeys[code].isDown = false;
	inputKeys[code].isUp = false;
}