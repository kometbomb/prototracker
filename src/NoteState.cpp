#include "NoteState.h"
#include "EffectParam.h"
#include "TrackState.h"
#include "PlayerState.h"
#include "SDL.h"
#include <math.h>

NoteState::NoteState()
	: frequency(0), slideTarget(0), volume(0)
{
	SDL_memset(effectMemory, 0, sizeof(effectMemory));
}


void NoteState::setFrequencyFromNote(int note)
{
	static const float ratio = powf(2.0f, 1.0f/12.0f);
	frequency = powf(ratio, note - (12 * 3 + 9));
}


void NoteState::setSlideTargetFromNote(int note)
{
	static const float ratio = powf(2.0f, 1.0f/12.0f);
	slideTarget = powf(ratio, note - (12 * 3 + 9));
}


bool NoteState::handleEffectZeroTick(const EffectParam& effect, TrackState& trackState, PlayerState& playerState)
{
	int asByte = effect.getParamsAsByte();
	
	switch (effect.effect)
	{
		case 'c':
			volume = asByte;
			
			if (volume > TrackState::maxVolume)
				volume = TrackState::maxVolume;
			break;
			
		case 'f':
			if (asByte < 0x20)
				playerState.songSpeed = asByte;
			else
				playerState.songRate = asByte;
			break;
			
		case 'j':
			trackState.macroRow = asByte;
			return true;
			
		case 'm':
			trackState.macro = asByte;
			trackState.macroRow = 0;
			break;
			
		case 's':
			trackState.macroSpeed = asByte;
			break;
			
		case 'w':
			trackState.wave = asByte;
			trackState.queuedWave = asByte;
			break;
			
		case 'q':
			trackState.queuedWave = asByte;
			break;
	}
	
	return false;
}


void NoteState::handleEffectAnyTick(const EffectParam& effect, TrackState& trackState, PlayerState& playerState)
{
	int param1 = effect.param1;
	int param2 = effect.param2;
	int asByte = effect.getParamsAsByte();
	
	if (asByte != 0)
		effectMemory[effect.effect] = asByte;
	else
	{
		asByte = effectMemory[effect.effect];
		param1 = (asByte & 0xf) >> 4;
		param2 = asByte & 0xf;
	}
	
	switch (effect.effect)
	{
		case 'a':
			volume -= param2;
			
			if (volume < 0)
				volume = 0;
			
			volume += param1;
			
			if (volume > TrackState::maxVolume)
				volume = TrackState::maxVolume;
			
			break;
			
		case 'k':
			if (asByte == playerState.tick)
				volume = 0;
			break;
			
		case '1':
		{
			frequency += (float)asByte / slideDivider;

		}	
			break;
			
		case '2':
		{
			frequency -= (float)asByte / slideDivider;
	
			if (frequency < 0)
				frequency = 0;
		}	
			break;
			
		case '3':
		{
			if (slideTarget < frequency)
			{
				frequency -= (float)asByte / slideDivider;
				
				if (frequency < slideTarget)
					frequency = slideTarget;
			}
			else
			{
				frequency += (float)asByte / slideDivider;
				
				if (frequency > slideTarget)
					frequency = slideTarget;
			}
	
			
		}	
			break;
	}
}
