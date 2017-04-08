#pragma once

#include "IOscillator.h"

struct WaveStore;

#ifndef OVERSAMPLE
#define OVERSAMPLE 4
#endif

class Oscillator: public IOscillator
{
	const WaveStore* mWaveStore;
	
	int mWave, mQueuedWave;
	int mSpeed;
	int mPosition;
	int mVolume;
	
public:
	static const int volumeResolution = 8192;
	static const int oscillatorLength = 256;
	static const int oscillatorResolution = 4096;
	static const int oscillatorOversample = OVERSAMPLE;

	Oscillator();
	virtual ~Oscillator();
	
	virtual void handleTrackState(ITrackState& trackState);
	void setWaveStore(const WaveStore& sampleStore);
	void setPosition(int newPosition);
	void setWave(int wave);
	void queueWave(int wave);
	virtual void setFrequency(float frequency);
	virtual void setVolume(int volume);
	virtual void update(int numSamples);
	virtual void render(Sample16 *buffer, int numSamples, int offset = 0);
};
