#pragma once

#include "Label.h"

struct Song;

class SequenceInfo: public Label
{
public:
	SequenceInfo(EditorState& editorState);
	virtual ~SequenceInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
