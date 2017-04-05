#pragma once

#include "NoteState.h"

struct PatternRow;

struct TrackState
{
	NoteState trackState;
	NoteState macroState;
	
	bool enabled;
	int wave, queuedWave;
	int tick;
	int macro;
	int macroSpeed;
	int macroRow;
	
	TrackState();
	
	static const int maxVolume = 64;
};
