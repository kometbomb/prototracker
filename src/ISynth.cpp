#include "ISynth.h"
#include "IOscillator.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "SDL.h"


ISynth::ISynth()
{
	mOscillator = new IOscillator*[SequenceRow::maxTracks];
	mPreviousOscillatorOutput = new Sample16[oscillatorProbeLength * SequenceRow::maxTracks];
	
	SDL_memset(mPreviousOscillatorOutput, 0, sizeof(Sample16) * oscillatorProbeLength * SequenceRow::maxTracks);
}


ISynth::~ISynth()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		delete mOscillator[i];
	
	delete[] mOscillator;
	delete[] mPreviousOscillatorOutput;
}


IOscillator& ISynth::getOscillator(int i)
{
	return *mOscillator[i];
}


const Sample16* ISynth::getOscillatorProbe(int oscillator) const
{
	return &mPreviousOscillatorOutput[oscillatorProbeLength * oscillator];
}


void ISynth::update(int numSamples)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i]->update(numSamples);
}


void ISynth::render(Sample16 *buffer, int numSamples)
{
	SDL_memset(buffer, 0, sizeof(Sample16) * numSamples);
	SDL_memset(mPreviousOscillatorOutput, 0, sizeof(Sample16) * oscillatorProbeLength * SequenceRow::maxTracks);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		mOscillator[i]->render(mPreviousOscillatorOutput + oscillatorProbeLength * i, oscillatorProbeLength);
		
		Sample16* src = mPreviousOscillatorOutput + oscillatorProbeLength * i;
		Sample16* dest = buffer;
		
		for (int p = 0 ; p < oscillatorProbeLength && p < numSamples ; ++p)
		{
			dest->left += src->left;
			dest->right += src->right;
			dest++;
			src++;
		}
		
		mOscillator[i]->render(buffer + oscillatorProbeLength, numSamples - oscillatorProbeLength, oscillatorProbeLength);
	}
}
