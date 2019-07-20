#include "EditorState.h"
#include "FileSection.h"
#include <cstring>
#include <cstdio>

EditorState::EditorState()
	: macro(0), octave(4), followPlayPosition(true)
{

}


TrackEditorState::TrackEditorState()
	: currentRow(0), currentTrack(0), currentColumn(0), editSkip(1), blockStart(-1), blockEnd(-1)
{

}


FileSection * EditorState::pack()
{
	FileSection * state = FileSection::createSection("STAT");
	state->writeByte(0);

    #define EDITOR_STATE_VALUE(name) \
        state->writeDword(name);
	EDITOR_STATE_VALUES
    #undef EDITOR_STATE_VALUE

	state->writeDword(followPlayPosition);

	/*state->writeDword(copyBuffer.getLength());
	state->writePattern(copyBuffer);*/

	FileSection *trState = sequenceEditor.pack();
	state->writeSection(*trState);
	delete trState;

	trState = patternEditor.pack();
	state->writeSection(*trState);
	delete trState;

	trState = macroEditor.pack();
	state->writeSection(*trState);
	delete trState;

	state->writeString(audioDevice.c_str());

	return state;
}


bool EditorState::unpack(const FileSection& section)
{
	if (strcmp(section.getName(), "STAT") != 0)
		return false;

	int offset = 0;

	unsigned int version = section.readByte(offset);

	if (version == FileSection::invalidRead)
		return false;

    #define EDITOR_STATE_VALUE(name) \
        name = section.readDword(offset); \
	    if (name == FileSection::invalidRead) \
		    return false;
	EDITOR_STATE_VALUES
    #undef EDITOR_STATE_VALUE

	unsigned int followNr = section.readDword(offset);

	if (followNr == FileSection::invalidRead)
		return false;

	/*unsigned int copyBufNr = section.readDword(offset);

	if (copyBufNr == FileSection::invalidRead)
		return false;

	if (!section.readPattern(copyBuffer, offset))
		return false;

	//copyBuffer.setLength(copyBufNr);*/

	FileSection *trState = section.readSection(offset);

	if (!trState)
		return false;

	if (!sequenceEditor.unpack(*trState))
	{
		delete trState;
		return false;
	}

	delete trState;

	trState = section.readSection(offset);

	if (!trState)
		return false;

	if (!patternEditor.unpack(*trState))
	{
		delete trState;
		return false;
	}

	delete trState;

	trState = section.readSection(offset);

	if (!trState)
		return false;

	if (!macroEditor.unpack(*trState))
	{
		delete trState;
		return false;
	}

	delete trState;

	const char *tempAudioDevice = section.readString(offset);

	if (!tempAudioDevice)
		return false;

	followPlayPosition = followNr;
	audioDevice = tempAudioDevice;

	return true;
}


FileSection * TrackEditorState::pack()
{
	FileSection * state = FileSection::createSection("TSTA");
	state->writeByte(0);
	state->writeDword(currentRow);
	state->writeDword(currentTrack);
	state->writeDword(currentColumn);
	state->writeDword(editSkip);
	state->writeDword(blockStart + 1);
	state->writeDword(blockEnd + 1);

	return state;
}


bool TrackEditorState::unpack(const FileSection& section)
{
	if (strcmp(section.getName(), "TSTA") != 0)
		return false;

	int offset = 0;

	unsigned int version = section.readByte(offset);

	if (version == FileSection::invalidRead)
		return false;

	unsigned int currentRowNr = section.readDword(offset);

	if (currentRowNr == FileSection::invalidRead)
		return false;

	unsigned int currentTrackNr = section.readDword(offset);

	if (currentTrackNr == FileSection::invalidRead)
		return false;

	unsigned int currentColumnNr = section.readDword(offset);

	if (currentColumnNr == FileSection::invalidRead)
		return false;

	unsigned int editSkipNr = section.readDword(offset);

	if (editSkipNr == FileSection::invalidRead)
		return false;

	unsigned int blockStartNr = section.readDword(offset);

	if (blockStartNr == FileSection::invalidRead)
		return false;

	unsigned int blockEndNr = section.readDword(offset);

	if (blockEndNr == FileSection::invalidRead)
		return false;

	currentRow = currentRowNr;
	currentTrack = currentTrackNr;
	currentColumn = currentColumnNr;

	editSkip = editSkipNr;
	blockStart = static_cast<int>(blockStartNr) - 1;
	blockEnd = static_cast<int>(blockEndNr) - 1;

	return true;
}


void EditorState::reset()
{
	sequenceEditor = TrackEditorState();
	patternEditor = TrackEditorState();
	macroEditor = TrackEditorState();

	copyBuffer.clear();

	followPlayPosition = true;

    // Default all editor state values to zero, override below if needed

	#define EDITOR_STATE_VALUE(name) \
        name = 0;
	EDITOR_STATE_VALUES
    #undef EDITOR_STATE_VALUE

    // Override octave default
	octave = 4;
	audioDevice = "";
}
