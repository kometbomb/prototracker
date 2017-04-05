#pragma once

#include "Pattern.h"

struct PatternRow;

class Macro: public Pattern
{
public:
	static const int macroNameLength = 8;
	
	char mName[macroNameLength + 1];
	
public:
	Macro();
	
	char *getName();
	
	void clear();
	
	static const int maxMacroLength = Pattern::maxRows;
};
