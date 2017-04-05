#pragma once

#include "Label.h"

struct Song;

class OctaveInfo: public Label
{
public:
	OctaveInfo(EditorState& editorState);
	virtual ~OctaveInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
