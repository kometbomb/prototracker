#pragma once

struct Wave;

class WaveStore
{
	Wave *mWave;
public:
	static const int maxWaves = 256;

	WaveStore();
	~WaveStore();
	const Wave& getWave(int index) const;
};
