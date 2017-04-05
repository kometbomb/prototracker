#pragma once

#include "EffectParam.h"

struct PatternRow
{
	/* 
	 * Column order when a row has a note and an effect
	 */
	enum Column
	{
		NoteType,
		NoteParam1,
		NoteParam2,
		EffectType,
		EffectParam1,
		EffectParam2,
		NumColumns,
		
		Note = NoteType,
		Octave = NoteParam2
	};
	
	
	enum ColumnFlag 
	{
		FlagNote = 1 << Note,
		FlagOctave = 1 << Octave,
		FlagEffectType = 1 << EffectType,
		FlagEffectParam1 = 1 << EffectParam1,
		FlagEffectParam2 = 1 << EffectParam2,
		FlagEffect = FlagEffectType,
		FlagAllColumns = -1
	};
	
	PatternRow();
	
	static const char *getNoteName(int note);
	
	EffectParam note;
	EffectParam effect;
	
	bool shouldSkipParam1() const;
	int getNote() const;
	int getOctave() const;
	void clear(int flags = FlagAllColumns);
	int getNoteWithOctave() const;
	void setOctave(int octave);
	void setNoteAndOctave(int noteAndOctave);
		
	static const int NoNote = 0xff;
};
