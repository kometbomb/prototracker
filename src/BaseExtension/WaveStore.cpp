#include "WaveStore.h"
#include "Wave.h"
#include "Oscillator.h"

WaveStore::WaveStore()
{
	mWave = new Wave[maxWaves];
	for (int i = 0 ; i < maxWaves ; ++i)
	{
		int length = Oscillator::oscillatorLength;
		
		if (i == 255)
			length *= 32;
		
		mWave[i].init(length);
		mWave[i].generate(i);
	}
}


WaveStore::~WaveStore()
{
	delete[] mWave;
}


const Wave& WaveStore::getWave(int index) const
{
	return mWave[index];
}
