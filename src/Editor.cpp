#include "Editor.h"
#include "Renderer.h"
#include "Color.h"
#include <cstdio>


Editor::Editor(EditorState& editorState, bool wantsFocus)
	: mEditorState(editorState), mFocus(NULL), mModal(NULL), mIsDirty(true), mRedraw(true), mParent(NULL), mNumChildren(0), mWantsFocus(wantsFocus)
{
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


void Editor::drawChildren(Renderer& renderer, const SDL_Rect& area)
{
	for (int index = 0 ; index < mNumChildren ; ++index)
	{
		if (mChildren[index]->isDirty())
		{
			SDL_Rect childArea = mChildrenArea[index];
			childArea.x += area.x;
			childArea.y += area.y;
			
			renderer.setClip(childArea);
			mChildren[index]->draw(renderer, childArea);
		}
	}
}


void Editor::drawModal(Renderer& renderer, const SDL_Rect& area)
{
	if (mModal != NULL)
	{
		if (mModal->shouldRedrawBackground())
		{
			// Draw plain black background with white border
			renderer.clearRect(area, Color(0, 0, 0));
			renderer.drawRect(area, Color(255, 255, 255));
		}
		
		SDL_Rect modalContent = area;
		modalContent.x += 2;
		modalContent.y += 2;
		modalContent.w -= 4;
		modalContent.h -= 4;
		
		mModal->draw(renderer, modalContent);
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
	if (mModal == NULL)
	{
		this->onDraw(renderer, area);
		drawChildren(renderer, area);
	}
	else
	{
		SDL_Rect modalArea = { area.x + 16, area.y + 16, area.w - 32, area.h - 32 };
		drawModal(renderer, modalArea);
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
	/* Placeholder, just print to stdout
	 */
	 
	printf("[%s] %s\n", messageClass == MessageInfo ? "INFO" : "ERROR", message);
}
