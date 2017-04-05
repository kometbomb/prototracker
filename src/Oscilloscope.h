#pragma once

#include "Editor.h"

class Player;
struct Sample16;

class Oscilloscope: public Editor
{
	const Sample16 *mBuffer;
	int mBufferLength, mChannel;
	Player& mPlayer;
	
public:
	Oscilloscope(EditorState& editorState, Player& player, int channel);
	
	void setBuffer(const Sample16 *buffer, int length);
	
	virtual ~Oscilloscope();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual bool onEvent(SDL_Event& event);
};

