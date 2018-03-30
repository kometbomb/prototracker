#pragma once

#include "Value.h"
#include "CopyBuffer.h"
#include <string>

struct FileSection;

struct TrackEditorState
{
	Value currentRow;
	Value currentTrack;
	Value currentColumn;
	Value editSkip;

	int blockStart, blockEnd;

	TrackEditorState();

	FileSection * pack();
	bool unpack(const FileSection& section);
};

struct EditorState
{
	Value macro;
	Value octave;
	Value editMode;

	TrackEditorState sequenceEditor;
	TrackEditorState patternEditor;
	TrackEditorState macroEditor;

	CopyBuffer copyBuffer;

	bool followPlayPosition;
	std::string audioDevice;

	EditorState();

	FileSection * pack();
	bool unpack(const FileSection& section);

	// Should use this to reset state instead of assigning
	// to an empty EditorState() to avoid double frees in CopyBuffer
	void reset();
};
