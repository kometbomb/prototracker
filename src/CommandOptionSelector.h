#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct Mixer;
struct MainEditor;
struct Label;

#include "GenericSelector.h"

class CommandOptionSelector: public GenericSelector
{
	const MainEditor& mMainEditor;
	TextEditor *mFilterField;
	Label *mFilterLabel;
	char mFilter[200];

public:
	struct CommandOption: public Item {
		int value;
		CommandOption(int value);
	};

private:
	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);

	static bool caseInsensitiveFind(const char *haystack, const char *needle);

public:
	CommandOptionSelector(EditorState& editorState, const MainEditor& mainEditor);
	virtual ~CommandOptionSelector();

	/* Fill command list from MainEditor
	 */
	void populate(CommandDescriptor command);


	/* After the dialog finishes this will return the option
	 */
	const CommandOption& getSelectedOption() const;

	virtual void onModalStatusChange(bool isNowModal);
	virtual bool onEvent(SDL_Event& event);

	/**
	 * Use when registering commands
	 */

	void addIntItem(int value);
};