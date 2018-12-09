#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct TextEditor;
struct Label;
struct MessageBox;

#include <vector>
#include <string>

class GenericList: public Editor
{
	int mSelectedItem, mScrollPosition, mRowHeight, mTopMargin;

protected:
	struct Item {
	};

    void setTopMargin(int margin);
    void setRowHeight(int height);
	void selectItem(int index);
	void setScrollPosition(int position);
	const Item& getSelectedItem() const;
    const int getSelectedItemIndex() const;
	void getVisibleItems(int& first, int& last) const;

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected) = 0;
	virtual void onAreaChanged(const SDL_Rect& area);
    virtual void onSelectItem(const Item& item);
	virtual int getVisibleCount() const;

	void addItem(Item* newItem);
	void clearItems();
	void sortItems(bool (*comparator)(const Item* a, const Item* b));
	int findClickedItem(int x, int y) const;

private:
	std::vector<Item*> mItems;

public:
	GenericList(EditorState& editorState);
	virtual ~GenericList();

	virtual bool onEvent(SDL_Event& event);
	virtual void onDraw(Renderer& renderer, const SDL_Rect& area);
	virtual void onRendererMount(const Renderer& renderer);
};
