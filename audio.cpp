#include "audio.h"

std::string getSoundPath(int soundId)
{
	switch (soundId)
	{
	case SOUND_INVADER_DESTROYED: return "asset/invader_explosion.wav";
	case SOUND_PLAYER_FIRE: return "asset/shoot.wav";
	case SOUND_UFO: return "asset/ufo.wav";
	case SOUND_PLAYER_EXPLOSION: return "asset/player_explosion.wav";
	}
}