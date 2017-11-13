#pragma once

#include "TrackEditor.h"

struct Song;
struct SequenceRow;

class PatternEditor: public TrackEditor
{
protected:
	PatternRow& getPatternRow(int track, int row);

protected:
	virtual PatternRow& getCurrentPatternRow();
	virtual Pattern& getCurrentPattern(int track);
	virtual SequenceRow& getCurrentSequenceRow();
	virtual void findUnusedTrack(int track);
	virtual bool isRowActive(int track, int row) const;

public:
	PatternEditor(EditorState& editorState, IPlayer& player, Song& song);
	virtual ~PatternEditor();

	void setPattern(int track, int pattern);
	void setPatternRow(int row);
	void setSequenceRow(int row);

	virtual bool onEvent(SDL_Event& event);
};
