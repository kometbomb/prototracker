#include "GenericSelector.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include "MessageBox.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>

GenericSelector::GenericSelector(EditorState& editorState)
	: GenericList(editorState)
{
	mLabel = new Label(mEditorState);
	addChild(mLabel, 0, 0, 100, 8);

	strcpy(mTitle, "");
}


GenericSelector::~GenericSelector()
{
	clearItems();
	delete mLabel;
}


void GenericSelector::onRendererMount(const Renderer& renderer)
{
	mLabel->setColor(renderer.getTheme().getColor(Theme::ColorType::ModalTitleText));
	mLabel->setBackground(renderer.getTheme().getColor(Theme::ColorType::ModalTitleBackground));

	SDL_Rect area = mLabel->getArea();
	area.h = renderer.getFontHeight();
	mLabel->setArea(area);

	setRowHeight(renderer.getFontHeight());
}


void GenericSelector::setTitle(const char *title)
{
	strncpy(mTitle, title, titleSize);
	mLabel->setText(mTitle);
	setModal(NULL);
}


bool GenericSelector::onEvent(SDL_Event& event)
{
	if (mModal)
		return mModal->onEvent(event);

  	switch (event.type)
	{
		case SDL_KEYDOWN: {
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				reject(true);
				return true;
			}

			if (event.key.keysym.sym == SDLK_RETURN)
			{
				accept(false);
				return true;
			}
		} break;

		case SDL_MOUSEBUTTONDOWN: {
			int item = findClickedItem(event.button.x / SCALE, event.button.y / SCALE);
			if (event.button.clicks == 2 && item == getSelectedItemIndex())
			{
				accept(false);
                return true;
			}
		} break;
	}

	return GenericList::onEvent(event);
}


int GenericSelector::getId() const
{
	return mId;
}


void GenericSelector::setId(int id)
{
	mId = id;
}


void GenericSelector::onAreaChanged(const SDL_Rect& area)
{
	GenericList::onAreaChanged(area);

	SDL_Rect labelArea = mLabel->getArea();

	// Editor should handle the 2px modal margin
	labelArea.w = area.w;

	mLabel->setArea(labelArea);

    // Label + title
    setTopMargin(labelArea.h * 2);
}


void GenericSelector::onSelectItem(const Item& item)
{

}
