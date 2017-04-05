#include "WaveGen.h"
#include "Random.h"
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cstring>
#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

#define convertBits(value, bit, width) ((value >> bit) & ((1 << width) - 1))

WaveGen::WaveGen()
{
}


WaveGen::~WaveGen()
{
}


void WaveGen::initSeed(int seed)
{
	mCarrier = convertBits(seed, carrierBit, carrierWidth);
	mModulator = convertBits(seed, modulatorBit, modulatorWidth);
	mModulatorRatio = convertBits(seed, modulatorRatioBit, modulatorRatioWidth);
	mPulseWidth = convertBits(seed, pwBit, pwWidth);
	mBoost = convertBits(seed, boostBit, boostWidth);
	
	//printf("%03d c=%d m=%d r=%d pw=%d\n", seed, mCarrier, mModulator, mModulatorRatio, mPulseWidth);
}

float WaveGen::step(float in, int steps)
{
	return (float)(int)(in * (steps / 2)) / (steps / 2);
}


float WaveGen::sine(float phase)
{
	return sin(phase * M_PI * 2.0f);
}


float WaveGen::saw(float phase)
{
	return fmod(fmod(phase, 1.0f) + 1.0f, 1.0f) * 2.0f - 1.0f;
}


float WaveGen::square(float phase, float pw)
{
	return fmod(fmod(phase, 1.0f) + 1.0f, 1.0f) < pw ? -1.0f : 1.0f;
}


float WaveGen::zSquare(float phase, float pw)
{
	return fmod(fmod(phase, 1.0f) + 1.0f, 1.0f) < pw ? 0.0f : 1.0f;
}


void WaveGen::generate(int seed, int *data, int length, int amplitude)
{
	memset(data, 0, sizeof(int) * length);
	
	/*for (int i = 0 ; i < 10 ; ++i)
		printf("%.1f\n", step((float)i / 10, 10));*/

	
	Random rnd;
		initSeed(seed);
		
		float hold = 0;
		
		for (int _i = 0 ; _i < length ; ++_i)
		{
			int i = _i;
			
			const struct { float c, m; } ratio[1 << (modulatorRatioWidth)] = {
				{1, 1}, {1, 2}, {1, 3}, {2, 1},
			};
			
			float carrier, phase = static_cast<float>(i) / length, modPhase = 0.0f;
			float c = ratio[mModulatorRatio].c;
			float m = ratio[mModulatorRatio].m;
			
			const float pulseWidth[1 << (pwWidth)] = {0.5f, 0.333f, 0.25f, 0.125f};
			const float modifier[1 << (pwWidth)] = {0.01f, 0.25, 0.5f, 1.0f};
			const float modifierPhase[1 << (pwWidth)] = {0.01f, 0.25, 0.333f, 0.5f};
			
			switch (mModulator)
			{
				default:
					modPhase = 0;
					break;
					
				case 0:
					modPhase = sine(phase * m) * modifier[mPulseWidth];
					break;
			}
			
			switch (mCarrier)
			{
				default:
				case 0:
					carrier = sine(phase * c + modPhase);
					break;
					
				case 1:
					carrier = saw(phase * c + modPhase);
					break;
					
				case 2:
					carrier = square(phase * c + modPhase, pulseWidth[mPulseWidth]);
					break;
					
				case 3:
					switch (mModulator)
					{
						default:
						case 0:
							carrier = sine(fmod(phase * c / 4 + modPhase, 0.25f));
							break;
							
						case 1:
							carrier = step(rnd.rndf() * 2.0f - 1.0f, 16) * 2.0f + square(phase*c*4, pulseWidth[mPulseWidth]) * 2.0f;
							break;
							
						case 2:
							carrier = step(rnd.rndf() * 2.0f - 1.0f, 64) * 2.0f + sine(phase*c*4) * 2.0f;
							break;
							
						case 3:
							carrier = step(rnd.rndf() * 2.0f - 1.0f, 32) * 2.0f * zSquare(phase*c, pulseWidth[mPulseWidth]) + sine(phase);
							break;
					}
					break;
			}
			
			switch (mModulator)
			{
				default:
					break;
				case 1:
					carrier *= sine(phase * m + modifierPhase[mPulseWidth % 4]);
					break;
					
				case 2:
					carrier *= saw(phase * m + modifierPhase[mPulseWidth % 4]);
					break;
					
				case 3:
					carrier *= square(phase * m + modifierPhase[mPulseWidth % 4]);
					break;
			}
			
			//carrier += sin(phase) * mBoost / 8;
			
			data[_i] = std::min(static_cast<float>(amplitude), std::max(-static_cast<float>(amplitude), amplitude * carrier));
		}
	
}
