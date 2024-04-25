#pragma once

#include "util.h"

#include <cstdint>
#include <string>

namespace platform
{
	extern bool muteSound;

	[[nodiscard]] uint64_t getTicks();
	void createWindow(const int width, const int height);
	void updateWindow();
	[[nodiscard]] Rect getWindowBarDims();
	void toggleSound();
	void playSound(const int soundId, const int channelId);
	void stopChannel(const int channelId);
	void stopAllSounds();
	void addBuffTexture(const Rect& srcRect, const Rect& dstRect);
	void addBuffFont(const Point2D& dstPoint, const std::string text);
	void addBuffRect(const Rect& rect, uint32_t rgb, const bool filled);
	void clearBuff();
	void renderBuff();
	void exit();
};