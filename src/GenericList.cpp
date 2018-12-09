#include "GenericList.h"
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

GenericList::GenericList(EditorState& editorState)
	: Editor(editorState), mSelectedItem(0), mScrollPosition(0), mRowHeight(8), mTopMargin(0)
{

}


GenericList::~GenericList()
{
	clearItems();
}


void GenericList::onRendererMount(const Renderer& renderer)
{
	mRowHeight = renderer.getFontHeight();
}


void GenericList::setScrollPosition(int position)
{
	mScrollPosition = std::max(0, std::min(static_cast<int>(mItems.size()) - getVisibleCount(), position));
	setDirty(true);
}


void GenericList::selectItem(int index)
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


bool GenericList::onEvent(SDL_Event& event)
{
	if (mModal)
		return mModal->onEvent(event);

	switch (event.type)
	{
		case SDL_KEYDOWN: {
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
			if (item != -1)
			{
				selectItem(item);
                return true;
			}
			return false;
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


int GenericList::getVisibleCount() const
{
	return (mThisArea.h - mTopMargin - mRowHeight) / mRowHeight;
}


void GenericList::getVisibleItems(int& firstVisible, int& lastVisible) const
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


void GenericList::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	if (shouldRedrawBackground())
	{
		renderer.clearRect(area, Theme::ColorType::ModalBackground);
	}

	int firstVisible, lastVisible;

	getVisibleItems(firstVisible, lastVisible);

	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {area.x, (row - firstVisible) * mRowHeight + area.y + mTopMargin, area.w - 4, renderer.getFontHeight()};
		renderer.setClip(textArea);
		renderItem(renderer, textArea, *mItems[row], row == mSelectedItem);
	}

	renderer.unsetClip();

	// height * 2 because there is an element/empty space under the label
	int topElementsHeight = mTopMargin;
	int areaHeight = area.h - topElementsHeight;
	int scrollbarTop = areaHeight * firstVisible / std::max(1, static_cast<int>(mItems.size()));
	int scrollbarBottom = areaHeight * (lastVisible + 1) / std::max(1, static_cast<int>(mItems.size()));
	SDL_Rect scrollbarArea = {area.x + area.w - 3, area.y + topElementsHeight + scrollbarTop,
		2, scrollbarBottom - scrollbarTop};
	renderer.clearRect(scrollbarArea, Theme::ColorType::ScrollBar);
}


int GenericList::findClickedItem(int x, int y) const
{
	int firstVisible, lastVisible;

	getVisibleItems(firstVisible, lastVisible);

	SDL_Point point = { x, y };

	for (int row = firstVisible ; row <= lastVisible ; ++row)
	{
		SDL_Rect textArea = {mThisArea.x, (row - firstVisible) * mRowHeight + mThisArea.y + mTopMargin,
			mThisArea.w - 4, mRowHeight};

		if (pointInRect(point, textArea))
		{
			return row;
		}
	}

	return -1;
}


const GenericList::Item& GenericList::getSelectedItem() const
{
	return *mItems[mSelectedItem];
}


void GenericList::addItem(GenericList::Item* newItem)
{
	mItems.push_back(newItem);
}


void GenericList::clearItems()
{
	mSelectedItem = 0;
	mScrollPosition = 0;

	for (Item* item : mItems)
	{
		delete item;
	}

	mItems.clear();
}


void GenericList::sortItems(bool (*comparator)(const Item* a, const Item* b))
{
	std::sort(mItems.begin(), mItems.end(), comparator);
}


void GenericList::onAreaChanged(const SDL_Rect& area)
{
	// Reset scroll position because we need the modal area to calculate it
	mScrollPosition = 0;
}


void GenericList::onSelectItem(const Item& item)
{

}


void GenericList::setTopMargin(int margin)
{
    mTopMargin = margin;
}


void GenericList::setRowHeight(int height)
{
    mRowHeight = height;
}


const int GenericList::getSelectedItemIndex() const
{
    return mSelectedItem;
}
