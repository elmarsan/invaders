#pragma once

#include <string>

enum Sound
{
	SOUND_INVADER_DESTROYED,
	SOUND_PLAYER_FIRE,

	SOUND_COUNT,
};

[[nodiscard]] std::string getSoundPath(int soundId);