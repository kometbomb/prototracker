#include "FileSection.h"
#include "PatternRow.h"
#include "Pattern.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cmath>

FileSection* FileSection::createSection(const char *name)
{
	return new FileSection(name);
}


static int read4bytes(void *ptr)
{
	int value = 0;

	for (int i = 0 ; i < 4 ; ++i)
		value |= *(reinterpret_cast<unsigned char*>(ptr) + i) << (i * 8);

	return value;
}


static void write4bytes(void *ptr, unsigned int dword)
{
	for (int i = 0 ; i < 4 ; ++i)
		reinterpret_cast<unsigned char*>(ptr)[i] = (dword >> (i * 8)) & 255;
}


FileSection* FileSection::openSection(void *bytes, int size)
{
	/*
	 * Needs to be at least section name + lenght dword
	 */

	if (size < nameSize + sizeof(int))
		return NULL;

	int sectionSize = read4bytes(&reinterpret_cast<char*>(bytes)[nameSize]);

	if (sectionSize > size)
		return NULL;

	return new FileSection(reinterpret_cast<const char*>(bytes), reinterpret_cast<char*>(bytes) + nameSize + sizeof(int), sectionSize - nameSize - sizeof(int));
}


FileSection::FileSection(const char *name, void *bytes, int size)
	: mData(NULL), mPackedData(NULL), mBase64(NULL), mSize(size), mAllocated(size)
{
	memset(mName, 0, sizeof(mName));
	strncpy(mName, name, nameSize);

	if (bytes != NULL && size > 0)
	{
		mData = malloc(size);
		memcpy(mData, bytes, size);
	}
	else
	{
		mAllocated = allocateChunkSize;
		mData = malloc(mAllocated);
	}
}


FileSection::~FileSection()
{
	if (mPackedData != NULL)
		free(mPackedData);

	if (mData != NULL)
		free(mData);

	if (mBase64 != NULL)
		delete[] mBase64;
}


const char* FileSection::getName() const
{
	return mName;
}


void * FileSection::getData()
{
	return mData;
}


int FileSection::getSize() const
{
	return mSize;
}


void * FileSection::getPackedData()
{
	if (mPackedData)
		free(mPackedData);

	mPackedData = malloc(getSize() + nameSize + sizeof(int));

	memcpy(mPackedData, getName(), nameSize);

	int tempSize = getPackedSize();

	write4bytes(static_cast<char *>(mPackedData) + nameSize, tempSize);
	memcpy(static_cast<char *>(mPackedData) + nameSize + sizeof(int), getData(), getSize());

	return mPackedData;
}


int FileSection::getPackedSize() const
{
	return getSize() + nameSize + sizeof(int);
}


void FileSection::ensureAllocated(int newSize)
{
	if (mAllocated < newSize)
	{
		mAllocated = newSize + allocateChunkSize;
		mData = realloc(mData, mAllocated);
	}
}


unsigned int FileSection::readByte(int& offset) const
{
	if (offset < 0 || offset + sizeof(char) > mSize)
		return invalidRead;

	unsigned int returnValue = static_cast<unsigned char*>(mData)[offset];

	offset += sizeof(char);

	return returnValue;
}


int FileSection::readSignedByte(int& offset) const
{
	if (offset < 0 || offset + sizeof(char) > mSize)
		return invalidRead;

	int returnValue = static_cast<char*>(mData)[offset];

	offset += sizeof(char);

	return returnValue;
}


unsigned int FileSection::readDword(int& offset) const
{
	if (offset < 0 || offset + sizeof(unsigned int) > mSize)
		return invalidRead;

	unsigned int returnValue = read4bytes(&static_cast<char*>(mData)[offset]);

	offset += sizeof(unsigned int);

	return returnValue;
}


float FileSection::readFloat(int& offset) const
{
	if (offset < 0 || offset + sizeof(float) > mSize)
		return NAN;

	unsigned int returnValueRaw = read4bytes(&static_cast<char*>(mData)[offset]);

	offset += sizeof(float);

	return *reinterpret_cast<float*>(&returnValueRaw);
}


const char * FileSection::readString(int& offset) const
{
	for (int i = offset ; ; ++i)
	{
		if (i >= mSize)
			return NULL;

		if (static_cast<char*>(mData)[i] == '\0')
			break;
	}

	const char *returnValue = static_cast<const char *>(&static_cast<char*>(mData)[offset]);

	offset += strlen(returnValue) + 1;

	return returnValue;
}


void FileSection::writeByte(unsigned int byte)
{
	ensureAllocated(mSize + sizeof(char));

	static_cast<char*>(mData)[mSize] = byte;

	mSize += sizeof(char);
}


void FileSection::writeDword(unsigned int dword)
{
	ensureAllocated(mSize + sizeof(unsigned int));

	write4bytes(&static_cast<char*>(mData)[mSize], dword);

	mSize += sizeof(unsigned int);
}


void FileSection::writeFloat(float value)
{
	ensureAllocated(mSize + sizeof(unsigned int));

	write4bytes(&static_cast<char*>(mData)[mSize], *reinterpret_cast<unsigned int*>(&value));

	mSize += sizeof(unsigned int);
}


void FileSection::writeString(const char *str)
{
	int strSize = strlen(str) + 1;

	ensureAllocated(mSize + strSize);

	memcpy(&static_cast<char*>(mData)[mSize], str, strSize);

	mSize += strSize;
}


FileSection * FileSection::readSection(int& offset) const
{
	FileSection *section = FileSection::openSection(&static_cast<char*>(mData)[offset], mSize - offset);

	if (!section)
		return NULL;

	offset += section->getSize() + nameSize + sizeof(int);

	return section;
}


void FileSection::writeSection(FileSection& section)
{
	ensureAllocated(mSize + section.getPackedSize());

	memcpy(&static_cast<char*>(mData)[mSize], section.getPackedData(), section.getPackedSize());

	mSize += section.getPackedSize();
}


void FileSection::writePattern(Pattern& pattern)
{
	int lastUsed = pattern.getLastUsedRow();

	if (lastUsed >= 255)
	{
		// 256 would wrap around in 8-bit space
		// so we write 255 as a special value that means
		// the pattern has all 256 rows written (even if it uses just 255)

		lastUsed = 256;
		writeByte(255);
	}
	else
		writeByte(lastUsed);

	for (int columnIndex = PatternRow::Note ; columnIndex < PatternRow::NumColumns ; ++columnIndex)
	{
		int effectParam;
		PatternRow::Column column;

		PatternRow::translateColumnEnum(columnIndex, effectParam, column);

		for (int row = 0 ; row < lastUsed ; ++row)
		{
			PatternRow& patternRow = pattern.getRow(row);

			switch (column)
			{
				case PatternRow::NoteType:
					writeByte(patternRow.getNote().effect);
					break;

				case PatternRow::NoteParam1:
					writeByte(patternRow.getNote().getParamsAsByte());
					break;

				case PatternRow::EffectType:
					writeByte(patternRow.getEffect(effectParam).effect);
					break;

				case PatternRow::EffectParam1:
					writeByte(patternRow.getEffect(effectParam).getParamsAsByte());
					break;

				case PatternRow::EffectParam2:
					// Written with param1
					break;

				default:
					break;
			}
		}
	}
}


bool FileSection::readPattern(Pattern& pattern, int effectParamCount, int& offset) const
{
	int lastUsed = readByte(offset);

	if (lastUsed == invalidRead)
		return false;

	// 254 = read 254 rows
	// 255 = read full 256 rows

	if (lastUsed == 255)
		lastUsed = 256;

	for (int columnIndex = PatternRow::Note ; columnIndex < effectParamCount * 3 + 3 ; ++columnIndex)
	{
		for (int row = 0 ; row < lastUsed ; ++row)
		{
			PatternRow& patternRow = pattern.getRow(row);
			int temp = 0;
			int effectParam;
			PatternRow::Column column;

			PatternRow::translateColumnEnum(columnIndex, effectParam, column);

			if (column >= PatternRow::EffectType && effectParam >= PatternRow::effectParams)
			{
				// Read and forget excess bytes (this tracker uses less effect colums)
				// Note column always exists so only skip >= EffectType
				temp = readByte(offset);
				if (temp == invalidRead)
					return false;
			}
			else
			{
				switch (column)
				{
					case PatternRow::NoteType:
						temp = readByte(offset);
						if (temp == invalidRead)
							return false;
						patternRow.getNote().effect = temp;
						break;

					case PatternRow::NoteParam1:
						temp = readByte(offset);
						if (temp == invalidRead)
							return false;
						patternRow.getNote().setParamsFromByte(temp);
						break;

					case PatternRow::EffectType:
						temp = readByte(offset);
						if (temp == invalidRead)
							return false;
						patternRow.getEffect(effectParam).effect = temp;
						break;

					case PatternRow::EffectParam1:
						temp = readByte(offset);
						if (temp == invalidRead)
							return false;
						patternRow.getEffect(effectParam).setParamsFromByte(temp);
						break;

					default:
						break;
				}
			}
		}
	}

	return true;
}


const char * FileSection::getBase64()
{
	static const char encodingTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const int modTable[] = {0, 2, 1};

	if (mBase64 != NULL)
		delete[] mBase64;

	int packedSize = getPackedSize();
	int encodedSize = ((packedSize - 1) / 3) * 4 + 4;

	mBase64 = new char[encodedSize + 1];

	void *packedData = getPackedData();

	for (int i = 0, j = 0 ; i < packedSize ; )
	{
		uint32_t octetA = i < packedSize ? (unsigned char)static_cast<const char *>(packedData)[i++] : 0;
        uint32_t octetB = i < packedSize ? (unsigned char)static_cast<const char *>(packedData)[i++] : 0;
        uint32_t octetC = i < packedSize ? (unsigned char)static_cast<const char *>(packedData)[i++] : 0;

        uint32_t triple = (octetA << 0x10) | (octetB << 0x08) | octetC;

        mBase64[j++] = encodingTable[(triple >> (3 * 6)) & 0x3F];
        mBase64[j++] = encodingTable[(triple >> (2 * 6)) & 0x3F];
        mBase64[j++] = encodingTable[(triple >> (1 * 6)) & 0x3F];
        mBase64[j++] = encodingTable[(triple >> (0 * 6)) & 0x3F];
	}

	for (int i = 0; i < modTable[packedSize % 3]; i++)
        mBase64[encodedSize - 1 - i] = '=';

	mBase64[encodedSize] = '\0';

	return mBase64;
}
