#include "SequenceRow.h"

const int SequenceRow::maxTracks;

SequenceRow::SequenceRow()
{
	clear();
}


void SequenceRow::clear()
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		pattern[i] = 0;
}


bool SequenceRow::isEmpty() const
{
	for (int i = 0 ; i < SequenceRow::maxTracks ; ++i)
		if (pattern[i] != 0)
			return false;

	return true;
}
