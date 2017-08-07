#pragma once

struct Pattern;
struct Sequence;
struct Macro;
struct FileSection;
struct SectionListener;

class Song
{
public:
	static const int songNameLength = 16;
	static const int songVersion = 16;
	
	// Error codes returned by unpack()
	
	enum UnpackError {
		Success,		// Song was unpacked successfully
		NotASong,		// Data format unknown
		ErrorVersion,	// Version number higher than our version
		ErrorRead,		// Something went wrong while reading data
		SectionUnhandled	// Nobody handled a file section
	};
	
private:
	Sequence *sequence;
	Pattern *patterns;
	Macro *macros;
	
	int patternLength;
	int sequenceLength;
	
	char name[songNameLength + 1];
	
	struct SectionListenerInfo {
		int flags;
		const char *sectionId;
		SectionListener *listener;
	};
	
	static const int maxListeners = 32;
	SectionListenerInfo mListeners[maxListeners];
	int mNumListeners;
	
public:
	Song();
	~Song();
	
	bool addSectionListener(const char *sectionId, SectionListener *sectionListener, int flags);
	
	int getPatternLength() const;
	int getSequenceLength() const;
	void setPatternLength(int length);
	void setSequenceLength(int length);
	
	Sequence& getSequence();
	Pattern& getPattern(int pattern);
	Macro& getMacro(int macro);
	
	int getLastPatternUsed() const;
	int getLastMacroUsed() const;
	char *getSongName();
	
	void clear();
	FileSection* pack();
	UnpackError unpack(const FileSection& section);
	
	int getSize() const;

	static const int maxPatterns = 256;
	static const int maxSequenceRows = 256;
	static const int maxMacros = 256;
};
