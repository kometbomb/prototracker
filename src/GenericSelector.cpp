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
	: Editor(editorState), mSelectedItem(0)
{
	mLabel = new Label(editorState);
	mLabel->setColor(Color(0, 0, 0));
	mLabel->setBackground(Color(255, 255, 255));
	addChild(mLabel, 0, 0, 280, 8);
	strcpy(mTitle, "");
}


GenericSelector::~GenericSelector()
{
	clearItems();
	delete mLabel;
}


void GenericSelector::selectItem(int index)
{
	mSelectedItem = index;

	if (mSelectedItem >= static_cast<int>(mItems.size()))
		mSelectedItem = static_cast<int>(mItems.size()) - 1;

	if (mSelectedItem < 0)
		mSelectedItem = 0;

	if (mItems.size() > 0)
	{
		onSelectItem(*mItems[mSelectedItem]);
	}

	setDirty(true);
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
		case SDL_KEYDOWN:
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

			switch (event.key.keysym.sym)
			{
				case SDLK_UP:
					selectItem(mSelectedItem - 1);
					return true;

				case SDLK_DOWN:
					selectItem(mSelectedItem + 1);
					return true;
			}

			break;
	}

	return false;
}


void GenericSelector::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
	{
		renderer.clearRect(area, Color(0, 0, 0));

		/*SDL_Rect fieldArea = { area.x, area.y + 8, area.w, renderer.getFontHeight() };

		renderer.clearRect(fieldArea, Color(255, 0, 0));*/
	}

	int countVisible = (area.h - 8 - 8 - 8) / 8;
	int firstVisible = mSelectedItem - countVisible / 2;

	if (firstVisible < 0)
	{
		firstVisible = 0;
	}

	int lastVisible = firstVisible + countVisible;

	if (lastVisible >= mItems.size())
	{
		lastVisible = mItems.size() - 1;
		firstVisible = lastVisible - countVisible;
		if (firstVisible < 0)
		{
			firstVisible = 0;
		}
	}

	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - firstVisible) * 8 + area.y + 16, area.w, 8};
		renderItem(renderer, textArea, *mItems[row], row == mSelectedItem);
	}
}


const GenericSelector::Item& GenericSelector::getSelectedItem() const
{
	return *mItems[mSelectedItem];
}



int GenericSelector::getId() const
{
	return mId;
}


void GenericSelector::setId(int id)
{
	mId = id;
}


void GenericSelector::addItem(GenericSelector::Item* newItem)
{
	mItems.push_back(newItem);
}


void GenericSelector::clearItems()
{
	mSelectedItem = 0;
	for (Item* item : mItems)
	{
		delete item;
	}
	mItems.clear();
}


void GenericSelector::sortItems(bool (*comparator)(const Item* a, const Item* b))
{
	std::sort(mItems.begin(), mItems.end(), comparator);
}
