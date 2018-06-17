#include "CommandOptionSelector.h"
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

CommandOptionSelector::CommandOptionSelector(EditorState& editorState, const CommandDescriptor& command)
	: GenericSelector(editorState), mCommandDescriptor(command)
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


CommandOptionSelector::~CommandOptionSelector()
{
	delete mFilterField;
	delete mFilterLabel;
}


void CommandOptionSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}


void CommandOptionSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void CommandOptionSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
	const CommandOption& commandOption = static_cast<const CommandOption&>(item);
	Color color;

	if (isSelected)
		color = Color(255, 0, 0);

	renderer.clearRect(area, Color(0, 0, 0));

	int width = area.w / 8 - 10;

	renderer.renderTextV(area, color, "%d", commandOption.value);
}


bool CommandOptionSelector::caseInsensitiveFind(const char *haystack, const char *needle)
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


void CommandOptionSelector::populate()
{
	invalidateParent();

	clearItems();

	mCommandDescriptor.option(*this);

	selectItem(0);
}


const CommandOptionSelector::CommandOption& CommandOptionSelector::getSelectedOption() const
{
	return static_cast<const CommandOption&>(getSelectedItem());
}


CommandOptionSelector::CommandOption::CommandOption(int _value)
	: value(_value)
{
}


void CommandOptionSelector::onModalStatusChange(bool isNowModal)
{
	// Make sure text field will receive SDL_TEXTINPUTs
	// and disables them after dialog close
	mFilterField->setIsEditing(isNowModal);

	// Reset filter on modal open
	strcpy(mFilter, "");
}


bool CommandOptionSelector::onEvent(SDL_Event& event)
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


void CommandOptionSelector::addIntItem(int value)
{
	char temp[100];
	snprintf(temp, sizeof(temp), "%d", value);

	if (caseInsensitiveFind(temp, mFilter))
		addItem(new CommandOption(value));
}
