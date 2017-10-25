#pragma once

#include "Editor.h"

struct TooltipManager;
struct EditorState;

class TooltipDisplayer: public Editor
{
	TooltipManager& mTooltipManager;
public:
	TooltipDisplayer(EditorState& editorState, TooltipManager& manager);
	
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onUpdate(int ms);
};
