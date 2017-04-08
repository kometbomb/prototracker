#pragma once

#include "NoteState.h"

struct ITrackState
{
	NoteState trackState;
	NoteState macroState;
	
	bool enabled;
	int tick;
	int macro;
	int macroSpeed;
	int macroRow;
	
	ITrackState();
	virtual ~ITrackState();
	
	virtual bool handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState);
	virtual void handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState);
	
	static const int maxVolume = 64;
};
