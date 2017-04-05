#include "TextEditor.h"
#include "Renderer.h"
#include "Color.h"
#include <strings.h>

TextEditor::TextEditor(EditorState& editorState)
	: Editor(editorState), mCursorPosition(0), mIsEditing(false), mAlwaysShowCursor(false), mSolidBackground(true)
{
}


TextEditor::~TextEditor()
{
}


void TextEditor::setIsEditing(bool state)
{
	mIsEditing = state;
	
	if (state)
		SDL_StartTextInput();
	else 
		SDL_StopTextInput();
}


bool TextEditor::onEvent(SDL_Event& event)
{
	if (mIsEditing)
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_BACKSPACE)
				{
					handleBackspace();
					return true;
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN)
				{
					setIsEditing(false);
					setDirty(true);
					return true;
				}
#ifdef __EMSCRIPTEN__
				// In case Event.preventDefault() eats the SDL_TEXTINPUT event, we manually add the (ASCII) keypresses here
				else
				{
					if (event.key.keysym.sym < 127)
						typeText((std::string() + static_cast<char>(event.key.keysym.sym)).c_str());
					
					return true;
				}
#endif
				break;
			
			case SDL_TEXTINPUT:
				typeText(event.text.text);
				return true;
		}
	}
	else
	{
		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				setIsEditing(true);
				setDirty(true);
				return true;
				break;
				
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_RETURN)
				{
					setIsEditing(true);
					setDirty(true);
					return true;
				}
				break;
		}
	}

	return false;
}


void TextEditor::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	setDirty(false);
	
	if (hasFocus())
		renderer.clearRect(area, Color(255,0,0));
	else
	{
		if (mSolidBackground)
			renderer.clearRect(area, Color(0,0,0));
		else
			renderer.renderBackground(area);
	}
		
	renderer.renderText(area, Color(), mBuffer);
	
	if ((hasFocus() && mIsEditing) || mAlwaysShowCursor)
	{
		SDL_Rect cursor = { area.x + 8 * static_cast<int>(strlen(mBuffer)), area.y, 8, 8 };
		renderer.clearRect(cursor, Color(255,255,255));
	}
}


void TextEditor::setSolidBackground(bool state)
{
	mSolidBackground = state;
}


void TextEditor::setBuffer(char *buffer, int bufferSize)
{
	setDirty(true);
	mBuffer = buffer;
	mBufferSize = bufferSize;
}


void TextEditor::typeText(const char *text)
{
	strncat(mBuffer, text, mBufferSize - strlen(mBuffer) - 1);
	setDirty(true);
}


void TextEditor::handleBackspace()
{
	int length = strlen(mBuffer);
	
	if (length > 0)
	{
		mBuffer[length - 1] = '\0';
		setDirty(true);
	}
	
}


void TextEditor::setText(const char *text)
{
	strncpy(mBuffer, text, mBufferSize);
	setDirty(true);
}


void TextEditor::setAlwaysShowCursor(bool state)
{
	mAlwaysShowCursor = state;
	
}

