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
	note.setNoteAndOctave(noteAndOctave);
}


int PatternRow::getNoteWithOctave() const
{
	return note.getNoteWithOctave();
}


void PatternRow::setOctave(int octave)
{
	note.param2 = octave;
}


int PatternRow::getOctave() const
{
	return note.param2;
}


int PatternRow::getNote() const
{
	if (note.effect != 'n')
		return NoNote;
	
	return note.param1;
}


void PatternRow::clear(int flags)
{
	if (flags & FlagNote)
		note = EffectParam();
	
	if (flags & FlagEffectType)
		effect = EffectParam();
}


bool PatternRow::shouldSkipParam1() const
{
	return (note.effect == 'n');
}
