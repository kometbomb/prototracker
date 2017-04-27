#pragma once

#include "Editor.h"

struct MessageManager;
struct EditorState;

class MessageDisplayer: public Editor
{
	MessageManager& mMessageManager;
public:
	MessageDisplayer(EditorState& editorState, MessageManager& manager);
	
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onUpdate(int ms);
};
