#include "TouchRegion.h"
#include "SDL.h"
#include <stdio.h>

TouchRegion::TouchRegion(EditorState& editorState, SDL_Scancode scancode, SDL_Keymod keymod)
	: Editor(editorState, false), mScancode(scancode), mKeymod(keymod)
{
	mKeycode = SDL_GetKeyFromScancode(scancode);
}


bool TouchRegion::onEvent(SDL_Event& event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
	{
		SDL_Event keyhit = {0};
		keyhit.type = event.type == SDL_MOUSEBUTTONUP ? SDL_KEYUP : SDL_KEYDOWN;
		keyhit.key.keysym.mod = mKeymod;
		keyhit.key.keysym.scancode = mScancode;
		keyhit.key.keysym.sym = mKeycode;
		
		SDL_PushEvent(&keyhit);
		return true;
	}
	
	return false;
}


void TouchRegion::onDraw(Renderer& renderer, const SDL_Rect& area)
{
}
