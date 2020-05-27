#include "Label.h"
#include "Renderer.h"
#include "Color.h"
#include <cstring>
#include <cstdarg>
#include <cstdio>

Label::Label(EditorState& editorState)
	: Editor(editorState, false), mTextColor(255, 255, 255), mBackgroundColor(0, 0, 0)
{
	mBuffer = new char[maxTextLength + 1];
	setText("");
}


Label::~Label()
{
	delete[] mBuffer;
}


void Label::setColor(const Color& color)
{
	mTextColor = color;
}


void Label::setBackground(const Color& color)
{
	mBackgroundColor = color;
}


void Label::setText(const char *text)
{
	strncpy(mBuffer, text, maxTextLength);
	setDirty(true);
}


void Label::setTextV(const char *text, ...)
{
	va_list argptr;
    va_start(argptr, text);
    vsnprintf(mBuffer, maxTextLength, text, argptr);
    va_end(argptr);

	setDirty(true);
}


bool Label::onEvent(SDL_Event& event)
{
	return false;
}


void Label::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);

	renderer.clearRect(area, mBackgroundColor);

	renderer.renderText(area, mTextColor, mBuffer);
}
