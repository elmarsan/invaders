#include "audio.h"

std::string getSoundPath(int soundId)
{
	switch (soundId)
	{
	case SOUND_INVADER_DESTROYED: return "asset/invaderkilled.wav";
	case SOUND_PLAYER_FIRE: return "asset/shoot.wav";
	}
}