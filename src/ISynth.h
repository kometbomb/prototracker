#pragma once

struct IOscillator;
struct Sample16;

/*

ISynth handles the IOscillators and some things needed by the GUI
(namely, data for visualization).

Override ISynth() to initialize your own IOscillators.

*/

class ISynth
{
protected:
	IOscillator **mOscillator;
	Sample16 *mPreviousOscillatorOutput;
	
public:
	ISynth();
	virtual ~ISynth();
	
	static const int oscillatorProbeLength = 128;

	const Sample16* getOscillatorProbe(int oscillator) const;
	IOscillator& getOscillator(int i);
	
	// Update the synth state
	void update(int numSamples);
	
	// Get samples (does not change synth state)
	void render(Sample16 *buffer, int numSamples);
};
