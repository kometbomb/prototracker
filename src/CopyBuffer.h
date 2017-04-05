#pragma once

#include "Pattern.h"

class CopyBuffer: public Pattern
{
protected:
	int mLength;
public:
	CopyBuffer();
	virtual ~CopyBuffer();
	
	void copy(Pattern& source, int start, int end);
	void paste(Pattern& destination, int start);
	
	int getLength() const;
	void setLength(int length);
};
