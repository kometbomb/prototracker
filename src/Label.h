#pragma once

#include "Editor.h"
#include "Color.h"

struct Renderer;

class Label: public Editor
{
	static const int maxTextLength = 100;

protected:
	char *mBuffer;
	Color mTextColor, mBackgroundColor;
	
public:
	Label(EditorState& editorState);
	
	void setColor(const Color& color);
	void setBackground(const Color& color);
	void setText(const char *text);
	void setTextV(const char *text, ...) __attribute__((format(printf, 2, 3)));
	
	virtual ~Label();
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual bool onEvent(SDL_Event& event);
};
