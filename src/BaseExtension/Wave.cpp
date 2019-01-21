#include "Wave.h"
#include "Oscillator.h"
#include "WaveGen.h"
#include "Random.h"
#include <math.h>
#include <cstdlib>
#include <cstdio>

Wave::Wave()
	: mLength(0), mLevels(0), mData(NULL), mOffset(NULL)
{
}


void Wave::init(int length)
{
	mLength = length;
	int totalSamples = 0;
	
	mLevels = 0;
	
	for (int l = length ; l > 0 ; l >>= 1)
		mLevels++;
	
	mOffset = new int[mLevels];
		
	for (int level = 0, l = length ; l > 0 ; l >>= 1, level++)
	{
		mOffset[level] = totalSamples;
		totalSamples += l;
	}
	
	mData = new int[totalSamples];
}


Wave::~Wave()
{
	delete[] mOffset;
	delete[] mData;
}

	
const int* Wave::getData(int level) const
{
	return mData + mOffset[level];
}


void Wave::generateMipMapLevel(int level)
{
	int length = getLength(level);
	int *dest = mData + mOffset[level];
	int *src = mData + mOffset[level - 1];
	
	//printf("Level %d: ", level);
	
	for (int i = 0 ; i < length ; ++i)
	{
		dest[i] = (src[i * 2] + src[i * 2 + 1]) / 2;
		
		//printf("%d ", dest[i]);
	}
	
	//printf("\n");
}


void Wave::generateMipMap()
{
	for (int i = 1 ; i < mLevels ; ++i)
		generateMipMapLevel(i);
}


int Wave::getLength(int level) const
{
	return mLength >> level;
}


int Wave::getLevels() const
{
	return mLevels;
}


void Wave::generate(int index)
{
	if (index < 16)
		generateSquare(0.5f + (float)index / 16 * 0.5);
	else if (index == 255)
	{
		generateNoise();
	}
	else
	{
		WaveGen gen;
		gen.generate(index, mData, mLength, waveAmplitude / 2);
		//generateSquare(0.5f);
	}
	
	generateMipMap();
}


void Wave::generateSquare(float pw)
{
	for (int i = 0 ; i < mLength ; ++i)
		if (i < mLength * pw)
			mData[i] = -waveAmplitude/2;
		else
			mData[i] = waveAmplitude/2;
		
	
}


void Wave::generateSine()
{
	
}



void Wave::generateNoise()
{
	int hold = 0;
	
	Random rnd;
	
	for (int i = 0 ; i < mLength ; ++i)
	{
		if ((i & 3) == 0)
		{
			hold = rnd.rnd(-waveAmplitude / 2, waveAmplitude / 2);
		}
		
		mData[i] = hold;
	}
}


void Wave::generateSaw()
{
}


void Wave::generateTriangle(float step)
{
}
