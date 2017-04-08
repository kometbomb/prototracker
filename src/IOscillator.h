#pragma once

struct Sample16;
struct ITrackState;

class IOscillator
{
public:
	virtual ~IOscillator() {}
	
	// The oscillator should update its own parameters from handleTrackState()
	virtual void handleTrackState(ITrackState& trackState);
	
	/*
	IOscillator needs to implement these.
	*/
	
	// Set oscillator frequency. 1.0 = 440 Hz.
	virtual void setFrequency(float frequency) = 0;
	
	// Needs to be implemented since the player uses this to mute/solo channels
	virtual void setVolume(int volume) = 0;
	
	// Update internal oscillator state
	virtual void update(int numSamples) = 0;
	
	// Fill buffer with samples (do not change oscillator state)
	virtual void render(Sample16 *buffer, int numSamples, int offset = 0) = 0;
};
