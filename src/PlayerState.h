#pragma once

struct PlayerState
{
	enum Mode
	{
		Stop,
		Play,
		PlaySequenceRow
	};

	Mode mode;

	int tick;
	int songSpeed;
	int songRate;
	int sequenceRow;
	int patternRow;
	float cpuUse;

	int updateMask;

	PlayerState();

	enum Updated {
		SequenceRow = 1,
		PatternRow = 2,
		Tick = 4,
		OscillatorProbe = 8,
		PatternRelevant = SequenceRow|PatternRow,
		Any = -1
	};

	int getUpdated();
	void ackUpdated(int mask);
	void setUpdated(int mask);

	bool isPlaying() const;
	bool shouldAdvanceSequenceRow() const;
};
