#pragma once

struct SequenceRow;

class Sequence
{
	SequenceRow *rows;	
	
public:
	Sequence();
	~Sequence();
	
	void insertRow(int row, bool allTracks);
	void deleteRow(int row, bool allTracks);

	SequenceRow& getRow(int sequenceRow);
	int getLastSequenceRowUsed() const;
	void clear();
	
	static const int maxRows = 256;
};
