#pragma once

struct EffectParam
{
	EffectParam();
	
	int effect;
	int param1, param2;
	
	bool isEmpty() const;
	
	// Set param1 and param2 from a linear note value (and set effect to 'N')
	void setNoteAndOctave(int noteAndOctave);
	
	// Get a linear note value 0 = C-0, 12 = C-1 etc. if effect equals 'N'
	int getNoteWithOctave() const;
	
	// Unpack param1 and param2 (both 4-bit nibbles) from a single byte
	void setParamsFromByte(int byte);
	
	// Pack param1 and param2 (both 4-bit nibbles) in a single byte
	int getParamsAsByte() const;
};
