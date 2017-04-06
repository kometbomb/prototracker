#pragma once

// This class is used to generate the various one-cycle waveforms.
// Used to init the wavetable on app start, it's not used realtime!

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

	// Convert continuous input value into discrete steps
	// Basically the same as a "bit crush" effect
	static float step(float in, int steps);
	
	// Sine wave between -1..1
	static float sine(float phase);
	
	// Saw wave from -1 to 1
	static float saw(float phase);
	
	// Square wave alternating between -1 and 1 (variable pulse width)
	static float square(float phase, float pw = 0.5f);
	
	// Square wave alternating between 0 and 1 (variable pulse width)
	static float zSquare(float phase, float pw = 0.5f);
	
	// The seed number is divided into 1-2 bit integers that specify
	// various wave generator modes
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
