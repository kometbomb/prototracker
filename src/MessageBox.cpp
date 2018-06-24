#include "MessageBox.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include <cstdio>
#include <cstring>

MessageBox::MessageBox(EditorState& editorState)
	: Editor(editorState)
{
	mLabel = new Label(editorState);
	addChild(mLabel, 0, 0, 240, 8);
	strcpy(mTitle, "");
}


MessageBox::~MessageBox()
{
	delete mLabel;
}


void MessageBox::onRendererMount(const Renderer& renderer)
{
	SDL_Rect area = mLabel->getArea();
	area.h = renderer.getFontHeight();
	mLabel->setArea(area);
}


void MessageBox::setTitle(const char *title)
{
	strncpy(mTitle, title, titleSize);
	mLabel->setText(mTitle);
}


bool MessageBox::onEvent(SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
				case SDLK_RETURN:
				case SDLK_y:
					mParent->onMessageBoxEvent(*this, 1);
					return true;

				case SDLK_ESCAPE:
				case SDLK_n:
					mParent->onMessageBoxEvent(*this, 0);
					return true;
			}

			break;
	}

	return false;
}


void MessageBox::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
		renderer.clearRect(area, renderer.getTheme().getColor(Theme::ColorType::ModalBackground));
}

int MessageBox::getId() const
{
	return mId;
}


void MessageBox::setId(int id)
{
	mId = id;
}
