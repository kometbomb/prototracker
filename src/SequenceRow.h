#pragma once

struct SequenceRow
{
	static const int maxTracks = 4;
	
	int pattern[maxTracks];
	
	void clear();
	bool isEmpty() const;
	
	SequenceRow();
};



