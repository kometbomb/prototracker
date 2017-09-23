#include "Renderer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "Color.h"
#include <ctype.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include "Theme.h"
#include "App.h"

#ifndef SCALE
#define SCALE 2
#endif

#ifndef FULLSCREEN
#define FULLSCREEN 0
#endif

#ifdef __EMSCRIPTEN__
/* Emscripten 1.37.9 seems to have fixed this, no need to flip Y coord anymore
 */
//#define FLIP_CLIP_Y 1
#endif

Renderer::Renderer()
	: mWindow(NULL), mRenderer(NULL), mFont(NULL), mBackground(NULL), mIntermediateTexture(NULL)
{
	
}


Renderer::~Renderer()
{
	SDL_DestroyTexture(mFont);
	SDL_DestroyTexture(mBackground);
	SDL_DestroyTexture(mIntermediateTexture);
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
}


void Renderer::setColor(const Color& color)
{
	SDL_SetRenderDrawColor(mRenderer, color.r, color.g, color.b, color.a);
}


void Renderer::clearRect(const SDL_Rect& rect, const Color& color)
{
	setColor(color);
	SDL_RenderFillRect(mRenderer, &rect);
}


void Renderer::drawRect(const SDL_Rect& rect, const Color& color)
{
	setColor(color);
	SDL_RenderDrawRect(mRenderer, &rect);
}


void Renderer::renderTextV(const SDL_Rect& position, const Color& color, const char * text, ...)
{ 
	char dest[1024];
    va_list argptr;
    va_start(argptr, text);
    vsnprintf(dest, sizeof(dest), text, argptr);
    va_end(argptr);
    renderText(position, color, dest);
}


void Renderer::renderText(const SDL_Rect& position, const Color& color, const char * text)
{
	SDL_Rect charArea = { position.x, position.y, getFontWidth(), getFontHeight() };
	
	for (const char *c = text ; *c ; ++c)
	{
		renderChar(charArea, color, *c);
		
		charArea.x += charArea.w;
	}
}


void Renderer::renderChar(const SDL_Rect& position, const Color& color, int c)
{
	static const char *charmap = "0123456789abcdefghijklmnopqrstuvwxyz-#/:._<>,'\"!";
	
	c = tolower(c);
	SDL_SetTextureBlendMode(mFont, SDL_BLENDMODE_BLEND);
	
	Color tmp = color;
	
	if (tmp.r >= 255)
		tmp.r = 254;
	
	if (tmp.g >= 255)
		tmp.g = 254;
	
	if (tmp.b >= 255)
		tmp.b = 254;
	
	SDL_SetTextureColorMod(mFont, tmp.r, tmp.g, tmp.b);
	
	for (int i = 0 ; charmap[i] ; ++i)
	{
		if (charmap[i] == c)
		{
			SDL_Rect charPos = { i * getFontWidth(), 0, getFontWidth(), getFontHeight() };
			if (SDL_RenderCopy(mRenderer, mFont, &charPos, &position) != 0)
			{
				printf("%s\n", SDL_GetError());
			}
		}
	}
}


void Renderer::present()
{
	int windowW = 0, windowH = 0;
	SDL_GetWindowSize(mWindow, &windowW, &windowH);
	SDL_Rect fullscreen = {(windowW - getWindowArea().w * SCALE) / 2, (windowH - getWindowArea().h * SCALE) / 2, getWindowArea().w * SCALE, getWindowArea().h * SCALE};
	SDL_SetRenderTarget(mRenderer, NULL);
	SDL_Rect src = {0,0,mGuiWidth,mGuiHeight};
	
	//SDL_RenderSetClipRect(mRenderer, &fullscreen);
	SDL_RenderCopy(mRenderer, mIntermediateTexture, &src, &fullscreen);
	SDL_RenderPresent(mRenderer);
}

SDL_Rect Renderer::getWindowArea() const
{
	SDL_Rect rect = {0, 0, mGuiWidth,mGuiHeight};
	return rect;
}


void Renderer::setClip(const SDL_Rect& area)
{
	SDL_Rect rect = area;
#ifdef FLIP_CLIP_Y
	rect.y = mGuiHeight - rect.y + rect.h;
#endif
	SDL_RenderSetClipRect(mRenderer, &rect);
}


void Renderer::renderRect(const SDL_Rect& rect, const Color& color, int index)
{
	SDL_SetTextureColorMod(mFont, color.r, color.g, color.b);
	SDL_Rect src = {index * 8,8,8,8};
	SDL_RenderCopy(mRenderer, mFont, &src, &rect);
}


void Renderer::renderBackground(const SDL_Rect& rect)
{
	SDL_SetTextureColorMod(mBackground, 255, 255, 255);
	SDL_RenderCopy(mRenderer, mBackground, &rect, &rect);
}


void Renderer::renderLine(int x1, int y1, int x2, int y2, const Color& color)
{
	setColor(color);
	SDL_RenderDrawLine(mRenderer, x1, y1, x2, y2);
}


void Renderer::beginRendering()
{
	// Start rendering, render to an intermediate texture for chunky scaled pixels
	SDL_SetRenderTarget(mRenderer, mIntermediateTexture);
}


void Renderer::renderPoints(const SDL_Point* points, int count, const Color& color)
{
	setColor(color);
	SDL_RenderDrawPoints(mRenderer, points, count);
}


int Renderer::getFontWidth() const
{
	return mFontWidth;
}


int Renderer::getFontHeight() const
{
	return mFontHeight;
}


bool Renderer::loadFont(const std::string& path, int charWidth, int charHeight)
{
	if (mFont != NULL)
		SDL_DestroyTexture(mFont);
	
	SDL_Surface *img = IMG_Load(path.c_str());
	
	if (!img)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Could not load font", ("Could not load "+path+". Perhaps you need to set the working directory?").c_str(), NULL);
		return false;
	}
	
	mFont = SDL_CreateTextureFromSurface(mRenderer, img);
	SDL_FreeSurface(img);
	
	mFontWidth = charWidth;
	mFontHeight = charHeight;
	
	return true;
}

	
bool Renderer::loadGui(const std::string& path, int width, int height)
{
	if (!mWindow)
		mWindow = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED,  SDL_WINDOWPOS_CENTERED, width * SCALE, height * SCALE, 0);
	
	if (!mRenderer)
	{
		mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_TARGETTEXTURE|SDL_RENDERER_SOFTWARE);
		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	}
	
	if (mBackground != NULL)
		SDL_DestroyTexture(mBackground);
	
	SDL_Surface *img = IMG_Load(path.c_str());
	
	if (!img)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Could not load GUI", ("Could not load "+path+". Perhaps you need to set the working directory?").c_str(), NULL);
		
		return false;
	}
	
	mBackground = SDL_CreateTextureFromSurface(mRenderer, img);
	SDL_FreeSurface(img);
	
#if FULLSCREEN
	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(0, &displayMode);
	SDL_SetWindowSize(mWindow, displayMode.w, displayMode.h);
	SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else	
	SDL_SetWindowSize(mWindow, width * SCALE, height * SCALE);
#endif
	SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	
	mGuiWidth = width;
	mGuiHeight = height;

	SDL_SetRenderTarget(mRenderer, NULL);
	
	if (mIntermediateTexture != NULL)
		SDL_DestroyTexture(mIntermediateTexture);
		
	mIntermediateTexture = SDL_CreateTexture(mRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, mGuiWidth, mGuiHeight);
	
	return true;
}


bool Renderer::setTheme(const Theme& theme)
{
	if (!loadGui(theme.getBackgroundPath(), theme.getWidth(), theme.getHeight()))
		return false;
	
	if (!loadFont(theme.getFontPath(), theme.getFontWidth(), theme.getFontHeight()))
		return false;
	
	return true;
}


void Renderer::scaleEventCoordinates(SDL_Event& event) const
{
#if FULLSCREEN
	SDL_DisplayMode displayMode;
	SDL_GetDesktopDisplayMode(0, &displayMode);
	
	switch (event.type)
	{
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEMOTION:
			event.button.x -= (displayMode.w - mGuiWidth * SCALE) / 2;
			event.button.y -= (displayMode.h - mGuiHeight * SCALE) / 2;
			break;
	}
#endif	
}


SDL_Rect Renderer::getTextRect(const char * text)
{
	SDL_Rect rect = {0, 0, mFontWidth * static_cast<int>(strlen(text)), mFontHeight};
	
	return rect;
}
