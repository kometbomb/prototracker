#include "Sequence.h"
#include "SequenceRow.h"
#include "Song.h"
#include <algorithm>

Sequence::Sequence()
{
	rows = new SequenceRow[Song::maxSequenceRows];
}


Sequence::~Sequence()
{
	delete[] rows;
}
	

int Sequence::getLastSequenceRowUsed() const
{
	int last = -1;
	
	for (int i = 0 ; i < Song::maxSequenceRows ; ++i)
	{
		if (!rows[i].isEmpty())
			last = std::max(last, i);
	}
	
	return last;
}


SequenceRow& Sequence::getRow(int sequenceRow)
{
	return rows[sequenceRow];
}


void Sequence::clear()
{
	for (int i = 0 ; i < Song::maxSequenceRows ; ++i)
	{
		rows[i].clear();
	}
}


void Sequence::insertRow(int row, bool allTracks)
{
	for (int i = maxRows - 1 ; i > row ; --i)
	{
		SequenceRow& src = rows[i - 1];
		SequenceRow& dest = rows[i];
		
		for (int track = 0 ; track < SequenceRow::maxTracks ; ++track)
			dest.pattern[track] = src.pattern[track];
	}
	
	rows[row].clear();
}


void Sequence::deleteRow(int row, bool allTracks)
{
	for (int i = row ; i < maxRows - 1 ; ++i)
	{
		SequenceRow& src = rows[i + 1];
		SequenceRow& dest = rows[i];
		
		for (int track = 0 ; track < SequenceRow::maxTracks ; ++track)
			dest.pattern[track] = src.pattern[track];
	}
	
	rows[maxRows - 1].clear();
}
