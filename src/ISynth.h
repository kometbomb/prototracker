#pragma once

struct IOscillator;
struct Sample16;

class ISynth
{
public:

    virtual ~ISynth() {}

	virtual void reset() = 0;

	static const int oscillatorProbeLength = 128;

	virtual const Sample16* getOscillatorProbe(int oscillator) const = 0;
	virtual IOscillator& getOscillator(int i) = 0;
	virtual int getProbePosition() const = 0;
	virtual void setSampleRate(int rate) = 0;

	// Update the synth state
	virtual void update(int numSamples) = 0;

	/*
	 Get samples (does not change synth state)
	 Note: SynthBase should overwrite the values in the buffer;
	 it might contain random values. Fill with zeroes if
	 the output should be silent.

	 This should also update mPreviousOscillatorOutput for
	 the GUI.
	*/
	virtual void render(Sample16 *buffer, int numSamples) = 0;
};
