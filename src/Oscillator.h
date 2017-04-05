#pragma once

struct Sample16;
struct WaveStore;

#ifndef OVERSAMPLE
#define OVERSAMPLE 4
#endif

class Oscillator
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
	~Oscillator();
	
	void setWaveStore(const WaveStore& sampleStore);

	void setPosition(int newPosition);
	void setWave(int wave);
	void queueWave(int wave);
	void setFrequency(float frequency);
	void setVolume(int volume);
	void update(int numSamples);
	void render(Sample16 *buffer, int numSamples, int offset = 0);
};
