#pragma once

#include "SDL.h"

struct Color;
struct Theme;

#include <string>
#include "Theme.h"

class Renderer
{
	SDL_Window *mWindow;
	SDL_Renderer *mRenderer;
	SDL_Texture *mFont, *mBackground, *mIntermediateTexture;
	int mGuiWidth, mGuiHeight, mFontWidth, mFontHeight;
	Theme mTheme;

	bool loadFont(const std::string& path, int charWidth, int charHeight);
	bool loadGui(const std::string& path, int width, int height);

public:
	Renderer();
	~Renderer();

	bool setTheme(const Theme& theme);
	const Theme& getTheme() const;
	void clearRect(const SDL_Rect& rect, const Color& color);
	void drawRect(const SDL_Rect& rect, const Color& color);
	void setClip(const SDL_Rect& area);
	void unsetClip();
	void setColor(const Color& color);
	void renderRect(const SDL_Rect& rect, const Color& color, int index = 0);
	void renderBackground(const SDL_Rect& rect);
	void renderText(const SDL_Rect& position, const Color& color, const char * text);
	SDL_Rect getTextRect(const char * text);
	void renderTextV(const SDL_Rect& position, const Color& color, const char * text, ...) __attribute__((format(printf, 4, 5)));
	void renderChar(const SDL_Rect& position, const Color& color, int c);
	void renderLine(int x1, int y1, int x2, int y2, const Color& color);
	void renderPoints(const SDL_Point* points, int count, const Color& color);
	void beginRendering();
	void present();
	int getFontWidth() const;
	int getFontHeight() const;
	SDL_Rect getWindowArea() const;
	void scaleEventCoordinates(SDL_Event& event) const;
};
