#include "ISynth.h"
#include "IOscillator.h"
#include "SequenceRow.h"
#include "Sample.h"
#include "SDL.h"
#include <algorithm>

const int ISynth::oscillatorProbeLength;

ISynth::ISynth()
	: mProbePosition(0)
{
	mOscillator = new IOscillator*[SequenceRow::maxTracks];
	mPreviousOscillatorOutput = new Sample16[oscillatorProbeLength * SequenceRow::maxTracks];
	mTempBuffer = new Sample16[2048];
	
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
	
	int probeCount = std::min(numSamples, oscillatorProbeLength);
	
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
	{
		SDL_memset(mTempBuffer, 0, sizeof(Sample16) * numSamples);
		mOscillator[i]->render(mTempBuffer, numSamples);
		
		for (int p = 0 ; p < numSamples ; ++p)
		{
			buffer[p].left += mTempBuffer[p].left;
			buffer[p].right += mTempBuffer[p].right;
		}
		
		Sample16 *src = mTempBuffer + std::max(0, numSamples - oscillatorProbeLength);
		Sample16 *dest = mPreviousOscillatorOutput + oscillatorProbeLength * i;
		
		for (int p = 0 ; p < probeCount ; ++p)
		{
			Sample16& sample = dest[(p + mProbePosition) % oscillatorProbeLength];
			sample.left = src->left;
			sample.right = src->right;
			src++;
		}
	}
	
	mProbePosition += probeCount;
}


int ISynth::getProbePosition() const
{
	return mProbePosition;
}


void ISynth::setSampleRate(int rate)
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		mOscillator[i]->setSampleRate(rate);
}


void ISynth::reset()
{
}
