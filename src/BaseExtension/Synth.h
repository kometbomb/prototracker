#pragma once

#include "../SynthBase.h"

struct WaveStore;

class Synth: public SynthBase
{
	WaveStore* mWaveStore;

public:
	Synth();
	virtual ~Synth();

	const WaveStore& getWaveStore() const;
};
