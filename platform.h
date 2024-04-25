#pragma once

#include "util.h"
#include <cstdint>

namespace platform
{
	[[nodiscard]] uint64_t getTicks();
	void createWindow(int width, int height);
	void updateWindow();
	[[nodiscard]] Rect getWindowBarDims();
	void playSound(int soundId);
	void addBuffTexture(const Rect& srcRect, const Rect& dstRect);
	void addBuffFont(const Point2D& dstPoint, const std::string text);
	void addBuffRect(const Rect& rect, uint32_t rgb, bool filled);
	void clearBuff();
	void renderBuff();
	void exit();
};