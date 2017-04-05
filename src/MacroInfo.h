#pragma once

#include "Label.h"

struct Song;

class MacroInfo: public Label
{
	Song& mSong;
	
public:
	MacroInfo(EditorState& editorState, Song& song);
	virtual ~MacroInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
