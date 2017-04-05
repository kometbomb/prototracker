#include "CopyBuffer.h"
#include "PatternRow.h"
#include <cstdio>

CopyBuffer::CopyBuffer()
	: Pattern(), mLength(0)
{
	
}


CopyBuffer::~CopyBuffer()
{
}

	
int CopyBuffer::getLength() const
{
	return mLength;
}


void CopyBuffer::setLength(int length)
{
	mLength = length;
}


void CopyBuffer::copy(Pattern& source, int start, int end)
{
	printf("Copy %d-%d\n", start, end);
	setLength(end - start + 1);
	
	for (int i = start, d = 0 ; i <= end ; ++i, ++d)
	{
		getRow(d) = source.getRow(i);
	}
}


void CopyBuffer::paste(Pattern& destination, int start)
{
	printf("Paste %d-%d\n", start, start + mLength - 1);
	for (int i = 0, d = start ; i < mLength ; ++i, ++d)
	{
		if (d >= Pattern::maxRows)
			break;
		destination.getRow(d) = getRow(i);
	}
}
