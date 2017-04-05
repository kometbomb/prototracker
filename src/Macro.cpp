#include "Macro.h"
#include "PatternRow.h"
#include <cstring>

Macro::Macro()
	: Pattern()
{
	strcpy(mName, "");
	//rows = new PatternRow[maxMacroLength];
}


char *Macro::getName()
{
	return mName;
}


void Macro::clear()
{
	Pattern::clear();
	
	strcpy(mName, "");
}
