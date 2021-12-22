#pragma once

#include "GenericList.h"

struct TextEditor;
struct Label;
struct MessageBox;

class GenericSelector: public GenericList
{
	static const int titleSize = 256;
	int mId;
	char mTitle[titleSize];

protected:
	Label *mLabel;

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected) = 0;
	virtual void onAreaChanged(const SDL_Rect& area);
	virtual void accept(bool isFinal = false) = 0;
	virtual void reject(bool isFinal = false) = 0;
	virtual void onSelectItem(const Item& item);

private:
	std::vector<Item*> mItems;

public:
	GenericSelector(EditorState& editorState);
	virtual ~GenericSelector();

	/* Set dialog id (freely set and used by the Editor that creates the dialog)
	 */
	void setId(int id);
	int getId() const;

	/* Set dialog title
	 */
	void setTitle(const char *title);

	virtual bool onEvent(SDL_Event& event);
	virtual void onRendererMount(const Renderer& renderer);
};
