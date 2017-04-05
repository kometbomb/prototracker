#pragma once

#include "Editor.h"

struct EditorState;

class TouchRegion: public Editor
{
	SDL_Scancode mScancode;
	SDL_Keycode mKeycode;
	SDL_Keymod mKeymod;
	
protected:
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	
public:
	TouchRegion(EditorState& editorState, SDL_Scancode scancode, SDL_Keymod keymod);
	virtual bool onEvent(SDL_Event& event);
};
