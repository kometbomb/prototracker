#pragma once

#include "Editor.h"

class IPlayer;
struct Sample16;

class Oscilloscope: public Editor
{
	const Sample16 *mBuffer;
	int mBufferLength, mChannel, mOscilloscopePos;
	IPlayer& mPlayer;
	
public:
	Oscilloscope(EditorState& editorState, IPlayer& player, int channel);
	
	void setBuffer(const Sample16 *buffer, int length);
	virtual void onListenableChange(Listenable *listenable);
	
	virtual ~Oscilloscope();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual bool onEvent(SDL_Event& event);
};

