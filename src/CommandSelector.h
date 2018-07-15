#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct Mixer;
struct MainEditor;
struct Label;

#include "GenericSelector.h"

class CommandSelector: public GenericSelector
{
	const MainEditor& mMainEditor;
	TextEditor *mFilterField;
	Label *mFilterLabel;
	char mFilter[200];

	struct CommandItem: public Item {
		const Editor::CommandDescriptor& command;
		static bool sort(const Item* a, const Item* b);
		CommandItem(const Editor::CommandDescriptor& command);
	};

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);

	static bool caseInsensitiveFind(const char *haystack, const char *needle);

public:
	CommandSelector(EditorState& editorState, const MainEditor& mainEditor);
	virtual ~CommandSelector();

	/* Fill command list from MainEditor
	 */
	void populate();


	/* After the dialog finishes this will return the command
	 */
	const CommandDescriptor& getSelectedCommand() const;

	virtual void onModalStatusChange(bool isNowModal);
	virtual bool onEvent(SDL_Event& event);
	virtual void onRendererMount(const Renderer& renderer);
};
