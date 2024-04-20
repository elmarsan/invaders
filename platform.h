#pragma once

#include "rect.h"

#include <cstdint>

extern bool running;
extern bool paused;

namespace platform
{
	[[nodiscard]] uint64_t getTicks();
	void createWindow(int width, int height);
	void updateWindow();
	void playSound(int soundId);
	void addBuffTexture(const Rect& srcRect, const Rect& dstRect);
	void addBuffRect(const Rect& rect, uint32_t rgb);
	void clearBuff();
	void renderBuff();
	void exit();
};