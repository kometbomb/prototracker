#pragma once

#include "ITrackState.h"

struct TrackState: public ITrackState
{
	int wave, queuedWave;
	
	TrackState();
	
	virtual bool handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState);
	virtual void handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState);
};

