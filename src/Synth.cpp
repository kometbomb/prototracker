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
	
	/* 
	
	Initialize the audio tracks.
	
	*/
	
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
	
	/*
	
	NOTE: ~ISynth() will delete the Oscillator objects we initialized
	above! No need to cleanup yourself.
	
	*/
}


const WaveStore& Synth::getWaveStore() const
{
	return *mWaveStore;
}
