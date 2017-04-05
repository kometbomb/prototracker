#pragma once

#include "Label.h"

struct Song;

class PatternLengthInfo: public Label
{
	Song& mSong;
	
public:
	PatternLengthInfo(EditorState& editorState, Song& song);
	virtual ~PatternLengthInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
