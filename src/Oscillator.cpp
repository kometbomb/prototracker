#include "Oscillator.h"
#include <math.h>
#include <cstdio>
#include "SDL.h"
#include "Sample.h"
#include "WaveStore.h"
#include "SequenceRow.h"
#include "Wave.h"
#include "TrackState.h"

Oscillator::Oscillator()
	: mWaveStore(NULL), mWave(0), mSpeed(0), mVolume(1.0), mPosition(0)
{
}


Oscillator::~Oscillator()
{
}


void Oscillator::setWaveStore(const WaveStore& waveStore)
{
	mWaveStore = &waveStore;
}


void Oscillator::setPosition(int newPosition)
{
	mPosition = newPosition;
}


void Oscillator::setWave(int wave)
{
	mWave = wave;
	mQueuedWave = wave;
}


void Oscillator::queueWave(int wave)
{
	mQueuedWave = wave;
}


void Oscillator::setFrequency(float frequency)
{
	mSpeed = frequency * oscillatorResolution * oscillatorLength*2;
}


void Oscillator::setVolume(int volume)
{
	mVolume = volume * volumeResolution / TrackState::maxVolume / SequenceRow::maxTracks;
}


void Oscillator::update(int numSamples)
{
	const Wave& wave = mWaveStore->getWave(mWave);
	int level = 0; //trunc(log2((float)mSpeed / oscillatorResolution));
	int resolution = oscillatorResolution << level;
	
	if (level < 0) 
		level = 0;
	else if (level >= wave.getLevels())
		level = wave.getLevels() - 1;
	
	int waveLengthMaskRes = wave.getLength(level) * resolution - 1;
	
	mPosition = (mPosition + mSpeed * numSamples);
	
	// Oscillator looped, set the queued wave
	if (mPosition > waveLengthMaskRes)
	{
		setWave(mQueuedWave);
		mPosition &= waveLengthMaskRes;
	}
}


void Oscillator::render(Sample16 *buffer, int numSamples, int offset)
{
	const Wave* wave = &mWaveStore->getWave(mWave);
	
	// Sample mipmap level is always 0 since oversampling takes care of aliasing
	
	int level = 0; //trunc(log2((float)mSpeed / oscillatorResolution));
	
	if (level < 0) 
		level = 0;
	else if (level >= wave->getLevels())
		level = wave->getLevels() - 1;
	
	int oversampleMul = 1 << oscillatorOversample;
	
	const int *waveData = wave->getData(level);
	int position = (mPosition + mSpeed * offset) * oversampleMul;
	
	/* 
	 * Scale the divider by the level
	 */
	int resolution = (oscillatorResolution << level) * oversampleMul;
	
	/*
	 * We assume that length is always 2^n
	 */
	int waveLengthMask = wave->getLength(level) - 1;
	int waveLengthMaskRes = wave->getLength(level) * resolution - 1;
	
	for (int i = 0 ; i < numSamples ; ++i)
	{
		int sample = 0;
		
		for (int o = 0 ; o < oversampleMul ; ++o)
		{
			/*int sampleA = waveData[(position / resolution) & waveLengthMask];
			int sampleB = waveData[(position / resolution + 1) & waveLengthMask];
			sample += (sampleB - sampleA) * (position % resolution) / resolution + sampleA;*/
		
			sample += waveData[(position / resolution) & waveLengthMask];
			position += mSpeed;
			
			// Oscillator looped, set the queued wave
			if (position > waveLengthMaskRes)
			{
				wave = &mWaveStore->getWave(mQueuedWave);
				waveData = wave->getData(level);
				position &= waveLengthMaskRes;
			}
		}
		
		sample = sample * mVolume / volumeResolution / oversampleMul;
		
		buffer[i].left += sample;
		buffer[i].right += sample;
	}
}
