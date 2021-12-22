#pragma once

#include "../Editor.h"

struct WaveStore;

class WaveView: public Editor
{
	const WaveStore& mWaveStore;
	int mX, mY;
public:
	WaveView(EditorState& editorState, const WaveStore& waveStore);
	virtual ~WaveView();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual bool onEvent(SDL_Event& event);
};
