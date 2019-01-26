#pragma once

#include "../Label.h"

class PatchInfo: public Label
{
public:
	PatchInfo(EditorState& editorState);
	virtual ~PatchInfo();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
