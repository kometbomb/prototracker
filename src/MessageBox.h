#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct Label;

#include <vector>
#include <dirent.h>

class MessageBox: public Editor
{
	static const int titleSize = 256;

	int mId;
	char mTitle[titleSize];
	Label *mLabel;

public:
	MessageBox(EditorState& editorState);
	virtual ~MessageBox();

	void setId(int id);
	void setTitle(const char *title);
	int getId() const;

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onRendererMount(const Renderer& renderer);
};
