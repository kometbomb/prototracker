#include "Pattern.h"
#include "PatternRow.h"
#include <cstring>

const int Pattern::maxRows;

Pattern::Pattern()
{
	rows = new PatternRow[maxRows];
}


Pattern::~Pattern()
{
	delete [] rows;
}


PatternRow& Pattern::getRow(int row)
{
	return rows[row];
}


void Pattern::clear()
{
	for (int i = 0 ; i < maxRows ; ++i)
		rows[i].clear();
}


int Pattern::getLastUsedRow() const
{
	int last = -1;
	
	for (int i = 0 ; i < maxRows ; ++i)
	{
		const PatternRow& row = rows[i];
		if (!row.note.isEmpty() || !row.effect.isEmpty())
		{
			last = i;
		}
	}
	
	return last + 1;
}


int Pattern::getLastMacroUsed() const
{
	int last = -1;
	
	for (int i = 0 ; i < maxRows ; ++i)
	{
		const PatternRow& row = rows[i];
		if (row.effect.effect == 'm')
		{
			last = row.effect.getParamsAsByte();
		}
	}
	
	return last;
}


bool Pattern::isEmpty() const
{
	return getLastUsedRow() == 0;
}


void Pattern::insertRow(int row, int flags)
{
	for (int i = maxRows - 1 ; i > row ; --i)
	{
		PatternRow& src = rows[i - 1];
		PatternRow& dest = rows[i];
		
		if (flags & PatternRow::FlagNote)
		{
			dest.note = src.note;
		}
		
		if (flags & PatternRow::FlagEffect)
		{
			dest.effect = src.effect;
		}
	}
	
	rows[row].clear(flags);
}


void Pattern::deleteRow(int row, int flags)
{
	for (int i = row ; i < maxRows - 1 ; ++i)
	{
		PatternRow& src = rows[i + 1];
		PatternRow& dest = rows[i];
		
		if (flags & PatternRow::FlagNote)
		{
			dest.note = src.note;
		}
		
		if (flags & PatternRow::FlagEffect)
		{
			dest.effect = src.effect;
		}
	}
	
	rows[maxRows - 1].clear(flags);
}

