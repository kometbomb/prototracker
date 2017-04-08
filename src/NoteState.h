#pragma once

struct EffectParam;
struct ITrackState;
struct PlayerState;

struct NoteState
{
	static const int slideDivider = 2048;
	
	float frequency, slideTarget;
	int volume;
	
	int effectMemory[256];
	
	NoteState();
	
	/* 
	 * Return true if the effect should restart pattern/macro effect processing
	 * Used by Jxx to process the target row effects
	 */
	
	bool handleEffectZeroTick(const EffectParam& effect, ITrackState& trackState, PlayerState& playerState);
	void handleEffectAnyTick(const EffectParam& effect, ITrackState& trackState, PlayerState& playerState);
	
	void setFrequencyFromNote(int note);
	void setSlideTargetFromNote(int note);
};
