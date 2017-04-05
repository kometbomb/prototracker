#pragma once

class WaveGen
{
	int mCarrier, mModulator;
	int mModulatorRatio;
	int mPulseWidth, mBoost;
	void initSeed(int seed);
	
public:
	WaveGen();
	~WaveGen();
	
	void generate(int seed, int *data, int length, int amplitude);

	static float step(float in, int steps);
	static float sine(float phase);
	static float saw(float phase);
	static float square(float phase, float pw = 0.5f);
	static float zSquare(float phase, float pw = 0.5f);
	
	static const int pwBit = 0;
	static const int pwWidth = 2;
	static const int boostBit = 1;
	static const int boostWidth = 3;
	static const int carrierBit = 6;
	static const int carrierWidth = 2;
	static const int modulatorBit = 4;
	static const int modulatorWidth = 2;
	static const int modulatorRatioBit = 2;
	static const int modulatorRatioWidth = 2;
};
