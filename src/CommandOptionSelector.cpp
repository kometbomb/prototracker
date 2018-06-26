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


void CommandOptionSelector::onRendererMount(const Renderer& renderer)
{
	GenericSelector::onRendererMount(renderer);

	mFilterLabel->setColor(renderer.getTheme().getColor(Theme::ColorType::ModalTitleText));
	mFilterLabel->setBackground(renderer.getTheme().getColor(Theme::ColorType::ModalTitleBackground));

	SDL_Rect filterLabelArea = mFilterLabel->getArea();
	filterLabelArea.y = mLabel->getArea().h;
	filterLabelArea.w = renderer.getFontWidth();
	filterLabelArea.h = renderer.getFontHeight();
	mFilterLabel->setArea(filterLabelArea);

	SDL_Rect filterArea = mFilterField->getArea();
	filterArea.x = filterLabelArea.x + filterLabelArea.w;
	filterArea.y = filterArea.y;
	filterArea.w = mThisArea.w - filterArea.x;
	filterArea.h = renderer.getFontHeight();
	mFilterField->setArea(filterArea);
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
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

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
