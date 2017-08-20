#pragma once

struct Sample16;
struct ITrackState;

class IOscillator
{
protected:
	int mSampleRate;
public:
	static const int oscillatorResolution = 4096;

	virtual ~IOscillator();
	virtual void setSampleRate(int rate);
	
	// The oscillator should update its own parameters from handleTrackState()
	virtual void handleTrackState(ITrackState& trackState);
	
	/*
	IOscillator needs to implement these.
	*/
	
	// Note has been triggered
	virtual void triggerNote() = 0;
	
	// Set oscillator frequency. 1.0 = 440 Hz.
	virtual void setFrequency(float frequency) = 0;
	
	// Needs to be implemented since the player uses this to mute/solo channels
	// Range is [0..TrackState::maxVolume] (i.e. [0..64])
	virtual void setVolume(int volume) = 0;
	
	// Update internal oscillator state
	virtual void update(int numSamples) = 0;
	
	// Fill buffer with samples (do not change oscillator state)
	// NOTE: This method should add its output to the values already in the buffer
	virtual void render(Sample16 *buffer, int numSamples, int offset = 0) = 0;
};
