#include "Synth.h"
#include "Oscillator.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "WaveStore.h"
#include "SDL.h"

Synth::Synth()
	: ISynth()
{
	mWaveStore = new WaveStore();
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		Oscillator *oscillator = new Oscillator();
		oscillator->setWaveStore(*mWaveStore);
		mOscillator[i] = oscillator;
	}
}


Synth::~Synth()
{
	delete mWaveStore;
}


const WaveStore& Synth::getWaveStore() const
{
	return *mWaveStore;
}
