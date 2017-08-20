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
	Sample16 *mPreviousOscillatorOutput, *mTempBuffer;
	int mProbePosition;
	
public:
	ISynth();
	virtual ~ISynth();
	
	static const int oscillatorProbeLength = 128;

	const Sample16* getOscillatorProbe(int oscillator) const;
	IOscillator& getOscillator(int i);
	int getProbePosition() const;
	void setSampleRate(int rate);
	
	// Update the synth state
	virtual void update(int numSamples);
	
	/*
	 Get samples (does not change synth state)
	 Note: ISynth should overwrite the values in the buffer;
	 it might contain random values. Fill with zeroes if 
	 the output should be silent.
	 
	 This should also update mPreviousOscillatorOutput for
	 the GUI.
	*/
	virtual void render(Sample16 *buffer, int numSamples);
};
