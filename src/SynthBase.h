#pragma once

#include "ISynth.h"

/*

SynthBase handles the IOscillators and some things needed by the GUI
(namely, data for visualization).

Override SynthBase() to initialize your own IOscillators.

*/

class SynthBase: public ISynth
{
protected:
	IOscillator **mOscillator;
	Sample16 *mPreviousOscillatorOutput, *mTempBuffer;
	int mProbePosition;

public:
	SynthBase();
	virtual ~SynthBase();

	virtual void reset();

	static const int oscillatorProbeLength = 128;

	virtual const Sample16* getOscillatorProbe(int oscillator) const;
	virtual IOscillator& getOscillator(int i);
	virtual int getProbePosition() const;
	virtual void setSampleRate(int rate);

	// Update the synth state
	virtual void update(int numSamples);

	/*
	 Get samples (does not change synth state)
	 Note: SynthBase should overwrite the values in the buffer;
	 it might contain random values. Fill with zeroes if
	 the output should be silent.

	 This should also update mPreviousOscillatorOutput for
	 the GUI.
	*/
	virtual void render(Sample16 *buffer, int numSamples);
};
