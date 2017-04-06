#pragma once

#include "ISynth.h"

struct WaveStore;

class Synth: public ISynth
{
	WaveStore* mWaveStore;
	
public:
	Synth();
	virtual ~Synth();
	
	const WaveStore& getWaveStore() const;
};
