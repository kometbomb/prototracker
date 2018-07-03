#include "WaveView.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "Wave.h"
#include "WaveStore.h"


WaveView::WaveView(EditorState& editorState, const WaveStore& waveStore)
	: Editor(editorState), mWaveStore(waveStore), mX(0), mY(0)
{
}


WaveView::~WaveView()
{
}


void WaveView::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	const int thumbW = 32;
	const int thumbH = 24;

	for (int y = mY ; y < mY + 8 ; y++)
	{
		for (int x = mX ; x < mX + 8 ; x++)
		{
			int w = x + y * 16;

			SDL_Rect rect = { area.x + (x - mX) * thumbW, area.y + (y - mY) * thumbH, thumbW, thumbH };
			renderer.clearRect(rect, Color(0,0,0));
			renderer.drawRect(rect, Color(255,255,255));

			const Wave& wave = mWaveStore.getWave(w);
			const int * data = wave.getData(0);

			int py = data[0] * (rect.h - 1) / Wave::waveAmplitude / 2;
			for (int x = 1 ; x < rect.w ; ++x)
			{
				int bufferPos = wave.getLength(0) * x / rect.w;
				int y = data[bufferPos] * (rect.h - 1) / Wave::waveAmplitude;
				renderer.renderLine(rect.x + x - 1, rect.y + rect.h / 2 + py, rect.x + x, rect.y + rect.h / 2 + y, Color());
				py = y;
			}

			renderer.renderTextV(rect, Color(), "%d", w);
		}
	}
}


bool WaveView::onEvent(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{
			case SDLK_UP:
				mY--;
				if (mY < 0)
					mY = 0;
				setDirty(true);
				return true;

			case SDLK_DOWN:
				mY++;
				if (mY > 8)
					mY = 8;
				setDirty(true);
				return true;

			case SDLK_LEFT:
				mX--;
				if (mX < 0)
					mX = 0;
				setDirty(true);
				return true;

			case SDLK_RIGHT:
				mX++;
				if (mX > 8)
					mX = 8;
				setDirty(true);
				return true;
		}
	}


	return false;
}
