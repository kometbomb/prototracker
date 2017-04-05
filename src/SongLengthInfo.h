#pragma once

#include "Label.h"

struct Song;

class SongLengthInfo: public Label
{
	Song& mSong;
	
public:
	SongLengthInfo(EditorState& editorState, Song& song);
	virtual ~SongLengthInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
