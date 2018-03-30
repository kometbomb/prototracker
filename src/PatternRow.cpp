#include "PatternRow.h"

PatternRow::PatternRow()
{
	clear();
}

const char *PatternRow::getNoteName(int noteIdx)
{
	if (noteIdx == NoNote)
		return "--";

	static const char *noteNames[] = {
		"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
	};

	return noteNames[noteIdx];
}


void PatternRow::setNoteAndOctave(int noteAndOctave)
{
	getNote().setNoteAndOctave(noteAndOctave);
}


int PatternRow::getNoteWithOctave() const
{
	return getNote().getNoteWithOctave();
}


void PatternRow::setOctave(int octave)
{
	getNote().param2 = octave;
}


int PatternRow::getOctave() const
{
	return getNote().param2;
}


int PatternRow::getNoteNr() const
{
	if (getNote().effect != 'n')
		return NoNote;

	return getNote().param1;
}


void PatternRow::clear(int flags)
{
	if (flags & FlagNote)
		getNote() = EffectParam();

	if (flags & FlagEffectType)
	{
		for (int i = 0 ; i < effectParams ; ++i)
		{
			getEffect(i) = EffectParam();
		}
	}
}


bool PatternRow::shouldSkipParam1() const
{
	return (getNote().effect == 'n');
}


void PatternRow::translateColumnEnum(int columnIndex, int& effectParam, PatternRow::Column& column)
{
	if (columnIndex < EffectType)
	{
		column = static_cast<Column>(columnIndex);
		effectParam = 0;
	}
	else
	{
		// columnIndex >= 3 repeats EffectType, Param1, Param2, EffectType, Param1...
		column = static_cast<Column>(columnIndex % 3 + EffectType);
		effectParam = columnIndex / 3 - 1;
	}
}


EffectParam& PatternRow::getNote()
{
	return effect[0];
}


EffectParam& PatternRow::getEffect(int index)
{
	return effect[index + 1];
}


const EffectParam& PatternRow::getNote() const
{
	return effect[0];
}


const EffectParam& PatternRow::getEffect(int index) const
{
	return effect[index + 1];
}


const EffectParam& PatternRow::getAnyParam(int index) const
{
	return effect[index];
}
