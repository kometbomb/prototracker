#pragma once

struct Macro;
struct Oscillator;

class Channel
{
	Oscillator *oscillator;
	Macro *macro;
	int note;
	
public:
	Channel();
	~Channel();
	
	void setMacro(Macro *macro);
	void setNote(int note);
	void updateTick();
	
	Oscillator& getOscillator();
};
