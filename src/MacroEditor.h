#pragma once

#include "TrackEditor.h"

struct Song;

class MacroEditor: public TrackEditor
{
protected:
	virtual PatternRow& getPatternRow(int track, int row);
	virtual PatternRow& getCurrentPatternRow();
	virtual Pattern& getCurrentPattern(int track);
	virtual void findUnusedTrack(int track);
	
public:
	MacroEditor(EditorState& editorState, Player& player, Song& song);
	virtual ~MacroEditor();
	
	virtual bool onEvent(SDL_Event& event);
};
