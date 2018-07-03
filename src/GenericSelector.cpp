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
	: Editor(editorState), mSelectedItem(0), mScrollPosition(0), mRowHeight(8)
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

	mRowHeight = renderer.getFontHeight();
}


void GenericSelector::setScrollPosition(int position)
{
	mScrollPosition = std::max(0, std::min(static_cast<int>(mItems.size()) - getVisibleCount(), position));
	setDirty(true);
}


void GenericSelector::selectItem(int index)
{
	mSelectedItem = index;

	if (mSelectedItem >= static_cast<int>(mItems.size()))
		mSelectedItem = static_cast<int>(mItems.size()) - 1;

	if (mSelectedItem < 0)
		mSelectedItem = 0;

	int countVisible = getVisibleCount();

	if (mSelectedItem < mScrollPosition)
	{
		mScrollPosition = mSelectedItem;
	}

	if (mSelectedItem > mScrollPosition + countVisible - 1)
	{
		mScrollPosition = std::max(mSelectedItem - countVisible + 1, 0);
	}

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

			int countVisible = getVisibleCount();

			switch (event.key.keysym.sym)
			{
				case SDLK_PAGEUP:
					selectItem(mSelectedItem - countVisible / 2);
					return true;

				case SDLK_PAGEDOWN:
					selectItem(mSelectedItem + countVisible / 2);
					return true;

				case SDLK_UP:
					selectItem(mSelectedItem - 1);
					return true;

				case SDLK_DOWN:
					selectItem(mSelectedItem + 1);
					return true;
			}

		} break;

		case SDL_MOUSEBUTTONDOWN: {
			int item = findClickedItem(event.button.x / SCALE, event.button.y / SCALE);
			if (event.button.clicks == 2 && item == mSelectedItem)
			{
				accept(false);
			}
			else if (item != -1)
			{
				selectItem(item);
			}
			return true;
		} break;

		case SDL_MOUSEWHEEL: {
			int flip = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;

			if (event.wheel.y < 0)
			{
				setScrollPosition(mScrollPosition + 4 * flip);
			}
			else if (event.wheel.y > 0)
			{
				setScrollPosition(mScrollPosition - 4 * flip);
			}
		} break;
	}

	return false;
}


int GenericSelector::getVisibleCount() const
{
	return (mThisArea.h - mLabel->getArea().h * 2 - mRowHeight) / mRowHeight;
}


void GenericSelector::getVisibleItems(int& firstVisible, int& lastVisible) const
{
	int countVisible = getVisibleCount();
	firstVisible = mScrollPosition;
	lastVisible = firstVisible + countVisible - 1;

	if (lastVisible >= mItems.size())
	{
		lastVisible = mItems.size() - 1;
		firstVisible = lastVisible - countVisible;
		if (firstVisible < 0)
		{
			firstVisible = 0;
		}
	}
}


void GenericSelector::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
	{
		renderer.clearRect(area, Theme::ColorType::ModalBackground);
	}

	int firstVisible, lastVisible;

	getVisibleItems(firstVisible, lastVisible);

	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - firstVisible) * mRowHeight + area.y + mLabel->getArea().h * 2, area.w - 4, renderer.getFontHeight()};
		renderer.setClip(textArea);
		renderItem(renderer, textArea, *mItems[row], row == mSelectedItem);
	}

	renderer.unsetClip();

	// height * 2 because there is an element/empty space under the label
	int topElementsHeight = mLabel->getArea().h * 2;
	int areaHeight = area.h - topElementsHeight;
	int scrollbarTop = areaHeight * firstVisible / std::max(1, static_cast<int>(mItems.size()));
	int scrollbarBottom = areaHeight * (lastVisible + 1) / std::max(1, static_cast<int>(mItems.size()));
	SDL_Rect scrollbarArea = {area.x + area.w - 3, area.y + topElementsHeight + scrollbarTop,
		2, scrollbarBottom - scrollbarTop};
	renderer.clearRect(scrollbarArea, Theme::ColorType::ScrollBar);
}


int GenericSelector::findClickedItem(int x, int y) const
{
	int firstVisible, lastVisible;

	getVisibleItems(firstVisible, lastVisible);

	SDL_Point point = { x, y };

	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {mThisArea.x, (row - firstVisible) * mRowHeight + mThisArea.y + mLabel->getArea().h * 2,
			mThisArea.w - 4, mRowHeight};

		if (pointInRect(point, textArea))
		{
			return row;
		}
	}

	return -1;
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
	mScrollPosition = 0;

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


void GenericSelector::onAreaChanged(const SDL_Rect& area)
{
	// Reset scroll position because we need the modal area to calculate it
	mScrollPosition = 0;

	SDL_Rect labelArea = mLabel->getArea();

	// Editor should handle the 2px modal margin
	labelArea.w = area.w;

	mLabel->setArea(labelArea);
}


void GenericSelector::onSelectItem(const Item& item)
{

}
