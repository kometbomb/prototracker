#pragma once
#include <cstdlib>

struct Pattern;

class FileSection
{
public:
	static const int nameSize = 4;

	/*
	 * readByte() & readDword() return this if it failed
	 */
	static const int invalidRead = 0xffffffff;

private:
	char mName[nameSize + 1];
	void* mData;
	void* mPackedData;
	char *mBase64;
	int mSize, mAllocated;

	static const int allocateChunkSize = 1024;

	/*
	 * Construct for reading (size > 0)/writing (size == 0)
	 */
	FileSection(const char *name, void *bytes = NULL, int size = 0);

	void ensureAllocated(int newSize);

public:

	static FileSection* openSection(void *bytes, int size);
	static FileSection* createSection(const char *name);

	~FileSection();

	const char *getName() const;
	void *getData();
	int getSize() const;
	void *getPackedData();
	int getPackedSize() const;

	const char * getBase64();

	unsigned int readByte(int& offset) const;
	int readSignedByte(int& offset) const;
	unsigned int readDword(int& offset) const;
	const char * readString(int& offset) const;
	FileSection * readSection(int& offset) const;
	bool readPattern(Pattern& pattern, int effectParamCount, int& offset) const;

	void writeByte(unsigned int byte);
	void writeDword(unsigned int dword);
	void writeString(const char *str);
	void writeSection(FileSection& section);
	void writePattern(Pattern& pattern);
};
