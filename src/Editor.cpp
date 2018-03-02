#include "Debug.h"
#include "Editor.h"
#include "Renderer.h"
#include "Color.h"
#include <cstdio>

#define MODAL_BORDER 2

Editor::Editor(EditorState& editorState, bool wantsFocus)
	: mEditorState(editorState), mFocus(NULL), mModal(NULL), mIsDirty(true), mRedraw(true), mParent(NULL), mNumChildren(0), mWantsFocus(wantsFocus)
{
	mThisArea.x = 0;
	mThisArea.y = 0;
}


Editor::~Editor() {}


Editor * Editor::getFocus()
{
	if (mParent)
		return mParent->getFocus();

	return mFocus;
}


void Editor::setFocus(Editor *editor)
{
	if (mParent)
		mParent->setFocus(editor);
	else
	{
		if (mFocus)
			mFocus->setDirty(true);
		mFocus = editor;

		if (editor)
			editor->setDirty(true);
	}
}


bool Editor::onEvent(SDL_Event& event)
{
	return false;
}


void Editor::setDirty(bool dirty)
{
	mIsDirty =  dirty;

	if (!dirty)
		mRedraw = false;

	if (dirty && mParent != NULL)
		mParent->setDirty(true);
}


bool Editor::shouldRedrawBackground() const
{
	return mRedraw;
}


bool Editor::isDirty() const
{
	return mIsDirty || hasDirty();
}


void Editor::addChild(Editor *child, int x, int y, int w, int h)
{
	child->mParent = this;
	SDL_Rect& area = mChildrenArea[mNumChildren];
	area.x = x;
	area.y = y;
	area.w = w;
	area.h = h;
	mChildren[mNumChildren++] = child;

	SDL_Rect absArea = {area.x + mThisArea.x, area.y + mThisArea.y, area.w, area.h};

	child->setArea(absArea);
}


void Editor::setArea(const SDL_Rect& area)
{
	mThisArea.x = area.x;
	mThisArea.y = area.y;
	mThisArea.w = area.w;
	mThisArea.h = area.h;

	if (mParent) mParent->childAreaChanged(this);

	onAreaChanged(mThisArea);
}


const SDL_Rect& Editor::getArea() const
{
	return mThisArea;
}


void Editor::onAreaChanged(const SDL_Rect& area)
{
}


bool Editor::hasDirty() const
{
	for (int i = 0 ; i < mNumChildren ; ++i)
		if (mChildren[i]->isDirty())
			return true;

	return false;
}


bool Editor::hasFocus()
{
	return getFocus() == this;
}



bool Editor::isFocusable() const
{
	return mWantsFocus;
}


void Editor::onListenableChange(Listenable *listenable)
{
	setDirty(true);
}


void Editor::drawCoveredChildren(Renderer& renderer, const SDL_Rect& area, const SDL_Rect& childArea, int maxIndex)
{
	renderer.renderBackground(childArea);

	for (int index = 0 ; index <= maxIndex && index < mNumChildren ; ++index)
	{
		SDL_Rect thisChildArea = mChildrenArea[index];
		thisChildArea.x += area.x;
		thisChildArea.y += area.y;

		SDL_Rect intersection;

		if (intersectRect(childArea, thisChildArea, intersection))
		{
			renderer.setClip(intersection);
			mChildren[index]->draw(renderer, thisChildArea);
		}
	}
}


void Editor::drawChildren(Renderer& renderer, const SDL_Rect& area)
{
	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		if (mChildren[index]->isDirty())
		{
			SDL_Rect childArea = mChildrenArea[index];
			childArea.x += area.x;
			childArea.y += area.y;

			drawCoveredChildren(renderer, area, childArea, index - 1);

			renderer.setClip(childArea);

			mChildren[index]->draw(renderer, childArea);
		}
	}
}


void Editor::drawModal(Renderer& renderer)
{
	if (mModal != NULL)
	{
		if (mModal->shouldRedrawBackground())
		{
			// Draw plain black background with white border
			SDL_Rect modalBorder = mModal->getArea();
			modalBorder.x -= MODAL_BORDER;
			modalBorder.y -= MODAL_BORDER;
			modalBorder.w += MODAL_BORDER * 2;
			modalBorder.h += MODAL_BORDER * 2;
			renderer.clearRect(modalBorder, Color(0, 0, 0));
			renderer.drawRect(modalBorder, Color(255, 255, 255));
		}

		mModal->draw(renderer, mModal->getArea());
	}
}


void Editor::setModal(Editor *modal)
{
	if (mModal != NULL)
		mModal->mParent = NULL;

	mModal = modal;

	if (mModal != NULL)
	{
		mModal->mParent = this;
		SDL_Rect modalArea = { mThisArea.x + 16, mThisArea.y + 16,
			mThisArea.w - 32, mThisArea.h - 32 };
		mModal->setArea(modalArea);
	}

	invalidateAll();
}


void Editor::onFileSelectorEvent(const Editor& fileSelector, bool accept)
{
}


void Editor::invalidateAll()
{
	setDirty(true);
	mRedraw = true;

	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		mChildren[index]->invalidateAll();
	}

	if (mModal)
		mModal->invalidateAll();
}


void Editor::onMessageBoxEvent(const Editor& messageBox, int code)
{
}


void Editor::draw(Renderer& renderer, const SDL_Rect& area)
{
	// This should fix problems with modal backgrounds not being updated
	// and perhaps also other child Editors.

	invalidateAll();

	if (mModal == NULL)
	{
		this->onDraw(renderer, area);
		drawChildren(renderer, area);
	}
	else
	{
		drawModal(renderer);
	}

	setDirty(false);
}


void Editor::invalidateParent()
{
	if (mParent)
		mParent->invalidateAll();
}


bool Editor::pointInRect(const SDL_Point& point, const SDL_Rect& rect)
{
#ifdef SDL_PointInRect
	return SDL_PointInRect(&point, &rect);
#else
	// In case we are using SDL <2.0.4 (of whatever the version is
	// where the rect built-in functions are introduced. E.g. my
	// PocketCHIP only goes up to 2.0.2 by default.

	return point.x >= rect.x && point.x < rect.x + rect.w &&
		point.y >= rect.y && point.y < rect.y + rect.h;
#endif
}


bool Editor::intersectRect(const SDL_Rect& a, const SDL_Rect& b, SDL_Rect& result)
{
#ifdef SDL_IntersectRect
	return SDL_IntersectRect(&point, &rect, &result);
#else
	// In case we are using SDL <2.0.4 (of whatever the version is
	// where the rect built-in functions are introduced. E.g. my
	// PocketCHIP only goes up to 2.0.2 by default.

	int aRight = a.x + a.w;
	int aBottom = a.y + a.h;
	int bRight = b.x + b.w;
	int bBottom = b.y + b.h;

	if (!(a.x < bRight && aRight > b.x && a.y < bBottom && aBottom > b.y))
		return false;

	result.x = std::max(a.x, b.x);
	result.y = std::max(a.y, b.y);
	result.w = std::min(aRight, bRight) - result.x;
	result.h = std::min(aBottom, bBottom) - result.y;

	return true;
#endif
}


void Editor::showTooltipV(const SDL_Rect& area, const char* message, ...)
{
	char dest[1024];
    va_list argptr;
    va_start(argptr, message);
    vsnprintf(dest, sizeof(dest), message, argptr);
    va_end(argptr);

	showTooltip(area, dest);
}


void Editor::showTooltip(const SDL_Rect& area, const char* message)
{
	if (mParent != NULL)
		mParent->showTooltip(area, message);
}


void Editor::showMessageV(MessageClass messageClass, const char* message, ...)
{
	char dest[1024];
    va_list argptr;
    va_start(argptr, message);
    vsnprintf(dest, sizeof(dest), message, argptr);
    va_end(argptr);

	showMessage(messageClass, dest);
}


void Editor::showMessage(MessageClass messageClass, const char* message)
{
	if (mParent != NULL)
		mParent->showMessage(messageClass, message);
	else
		debug("[%s] %s", messageClass == MessageInfo ? "INFO" : "ERROR", message);
}


void Editor::onUpdate(int ms)
{

}


void Editor::update(int ms)
{
	onUpdate(ms);

	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		mChildren[index]->update(ms);
	}
}


void Editor::onLoaded()
{
	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		mChildren[index]->onLoaded();
	}
}


void Editor::childAreaChanged(Editor *child)
{
	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		if (mChildren[index] == child)
			mChildrenArea[index] = child->getArea();
	}
}
