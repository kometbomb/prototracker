#pragma once

#include "NoteState.h"

/*

ITrackState handles the common track state. That is, things like macro state and channel mute
are handled here so the class extending ITrackState doesn't have to.

Override handleEffectAnyTick() and handleEffectZeroTick() to add your own behavior.

 */

struct ITrackState
{
	NoteState trackState;
	NoteState macroState;

	// Note was triggered during current tick
	// Set to false after handled.
	bool triggered;

	// Track is enabled (not muted)
	bool enabled;

	int tick;
	int macro;
	int macroSpeed;
	int macroRow;
    int patch;

	ITrackState();
	virtual ~ITrackState();

	virtual bool handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState);
	virtual void handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState);

	static const int maxVolume = 64;
};
