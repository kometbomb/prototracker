#include "CommandSelector.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include "MessageBox.h"
#include "MainEditor.h"
#include <cstdio>
#include <cstring>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include "Mixer.h"
#include "Label.h"

CommandSelector::CommandSelector(EditorState& editorState, const MainEditor& mainEditor)
	: GenericSelector(editorState), mMainEditor(mainEditor)
{
	mFilterField = new TextEditor(editorState);
	mFilterField->setBuffer(mFilter, sizeof(mFilter));
	mFilterField->setIsEditing(true);
	mFilterField->setAlwaysShowCursor(true);

	mFilterLabel = new Label(editorState);
	mFilterLabel->setColor(Color(0, 0, 0));
	mFilterLabel->setBackground(Color(255, 255, 255));
	mFilterLabel->setText(">");
	addChild(mFilterLabel, 0, 8, 8, 8);

	strcpy(mFilter, "");
	addChild(mFilterField, 8, 8, 208, 8);
	setFocus(mFilterField);
}


CommandSelector::~CommandSelector()
{
	delete mFilterField;
	delete mFilterLabel;
}


void CommandSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}


void CommandSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void CommandSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
	const CommandItem& commandItem = static_cast<const CommandItem&>(item);
	Color color;

	if (isSelected)
		color = Color(255, 0, 0);

	renderer.clearRect(area, Color(0, 0, 0));

	int width = area.w / 8 - 10;

	renderer.renderTextV(area, color, "%s", commandItem.command.name);
}


bool CommandSelector::caseInsensitiveFind(const char *haystack, const char *needle)
{
	// Empty needle, everything matches

	if (*needle == '\0')
		return true;

	const char *a = haystack;

	while (*a)
	{
		const char *prev = a, *b = needle;

		do
		{
			if (tolower(*b) != tolower(*a))
				break;

			++b;
			++a;

			if (*b == '\0')
				return true;
		}
		while (*a);

		a = prev;
		++a;
	}

	return false;
}


void CommandSelector::populate()
{
	invalidateParent();

	clearItems();

	int numCommands = mMainEditor.getNumCommands();
	for (int index = 0 ; index < numCommands ; ++index)
	{
		const auto& command = mMainEditor.getCommand(index);
		if (caseInsensitiveFind(command.name, mFilter))
			addItem(new CommandItem(command));
	}

	selectItem(0);
}


const Editor::CommandDescriptor& CommandSelector::getSelectedCommand() const
{
	return static_cast<const CommandItem&>(getSelectedItem()).command;
}


CommandSelector::CommandItem::CommandItem(const CommandDescriptor& descriptor)
	: command(descriptor)
{
}


void CommandSelector::onModalStatusChange(bool isNowModal)
{
	// Make sure text field will receive SDL_TEXTINPUTs
	// and disables them after dialog close
	mFilterField->setIsEditing(isNowModal);
}


bool CommandSelector::onEvent(SDL_Event& event)
{
	if (GenericSelector::onEvent(event))
		return true;

	bool filterEvent = mFilterField->onEvent(event);

	if (filterEvent)
	{
		populate();
	}

	return filterEvent;
}
