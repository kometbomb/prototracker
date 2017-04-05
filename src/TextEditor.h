#pragma once

#include "Editor.h"

class TextEditor: public Editor
{
	char *mBuffer;
	int mBufferSize;
	int mCursorPosition;
	bool mIsEditing;
	bool mAlwaysShowCursor;
	bool mSolidBackground;
	
	void typeText(const char *text);
	void handleBackspace();
	
public:
	TextEditor(EditorState& editorState);
	virtual ~TextEditor();

	void setText(const char *text);
	void setBuffer(char *buffer, int bufferSize);
	void setIsEditing(bool state);
	void setSolidBackground(bool state);
	void setAlwaysShowCursor(bool state);
	
	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
};
