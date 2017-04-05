#pragma once

struct Oscillator;
struct Sample16;
struct WaveStore;

class Synth
{
	WaveStore* mWaveStore;
	Oscillator* mOscillator;
	Sample16 *mPreviousOscillatorOutput;
	
public:
	Synth();
	~Synth();
	
	const WaveStore& getWaveStore() const;
	
	static const int oscillatorProbeLength = 128;

	const Sample16* getOscillatorProbe(int oscillator) const;
	Oscillator& getOscillator(int i);
	
	// Update the synth state
	void update(int numSamples);
	
	// Get samples (does not change synth state)
	void render(Sample16 *buffer, int numSamples);
};
