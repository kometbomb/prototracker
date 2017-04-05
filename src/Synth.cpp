#include "Synth.h"
#include "Oscillator.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "WaveStore.h"
#include "SDL.h"

Synth::Synth()
{
	mWaveStore = new WaveStore();
	mOscillator = new Oscillator[SequenceRow::maxTracks];
	mPreviousOscillatorOutput = new Sample16[oscillatorProbeLength * SequenceRow::maxTracks];
	
	SDL_memset(mPreviousOscillatorOutput, 0, sizeof(Sample16) * oscillatorProbeLength * SequenceRow::maxTracks);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i].setWaveStore(*mWaveStore);
}


Synth::~Synth()
{
	delete[] mOscillator;
	delete mWaveStore;
	delete[] mPreviousOscillatorOutput;
}
	

Oscillator& Synth::getOscillator(int i)
{
	return mOscillator[i];
}


const Sample16* Synth::getOscillatorProbe(int oscillator) const
{
	return &mPreviousOscillatorOutput[oscillatorProbeLength * oscillator];
}


void Synth::update(int numSamples)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i].update(numSamples);
}


void Synth::render(Sample16 *buffer, int numSamples)
{
	SDL_memset(buffer, 0, sizeof(Sample16) * numSamples);
	SDL_memset(mPreviousOscillatorOutput, 0, sizeof(Sample16) * oscillatorProbeLength * SequenceRow::maxTracks);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mOscillator[i].render(mPreviousOscillatorOutput + oscillatorProbeLength * i, oscillatorProbeLength);
		
		Sample16* src = mPreviousOscillatorOutput + oscillatorProbeLength * i;
		Sample16* dest = buffer;
		
		for (int p = 0 ; p < oscillatorProbeLength && p < numSamples ; ++p)
		{
			dest->left += src->left;
			dest->right += src->right;
			dest++;
			src++;
		}
		
		mOscillator[i].render(buffer + oscillatorProbeLength, numSamples - oscillatorProbeLength, oscillatorProbeLength);
	}
}


const WaveStore& Synth::getWaveStore() const
{
	return *mWaveStore;
}
