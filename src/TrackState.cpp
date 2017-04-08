#include "TrackState.h"
#include "EffectParam.h"

TrackState::TrackState()
	: ITrackState(), wave(0), queuedWave(0)
{

}


bool TrackState::handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState)
{
	int asByte = effect.getParamsAsByte();
	
	switch (effect.effect)
	{
		case 'w':
			wave = asByte;
			queuedWave = asByte;
			break;
			
		case 'q':
			queuedWave = asByte;
			break;
			
		default:
			return ITrackState::handleEffectZeroTick(effect, playerState);
	}
	
	return false;
}


void TrackState::handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState)
{
	ITrackState::handleEffectAnyTick(effect, playerState);
}
