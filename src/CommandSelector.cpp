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


void CommandSelector::onRendererMount(const Renderer& renderer)
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
	filterArea.y = filterLabelArea.y;
	filterArea.w = mThisArea.w - filterArea.x;
	filterArea.h = renderer.getFontHeight();
	mFilterField->setArea(filterArea);
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
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

	renderer.renderTextV(area, color, "%s: %s", commandItem.command.context, commandItem.command.name);

	if (commandItem.command.sym != -1)
	{
		const char *keyName = SDL_GetKeyName(commandItem.command.sym);
		SDL_Rect textRect = renderer.getTextRect(keyName);
		SDL_Rect shortcutArea = { area.x + area.w - textRect.w, area.y, textRect.w, textRect.h };
		renderer.clearRect(shortcutArea, Theme::ColorType::CommandShortcutBackground);
		renderer.renderTextV(shortcutArea, Theme::ColorType::CommandShortcut, keyName);

		if (commandItem.command.mod != 0)
		{
			static const struct { const char *name; int mod; } modMap[] = {
				{ "CTRL", KMOD_CTRL },
				{ "ALT", KMOD_ALT },
				{ "SHIFT", KMOD_SHIFT },
				{ NULL, 0 },
			};

			int left = shortcutArea.x;

			for (int i = 0 ; modMap[i].name ; ++i)
			{
				if (commandItem.command.mod & modMap[i].mod)
				{
					const char *keyName = modMap[i].name;
					SDL_Rect textRect = renderer.getTextRect(keyName);
					SDL_Rect modifierArea = { left - textRect.w - 4, area.y, textRect.w, textRect.h };
					renderer.clearRect(modifierArea, Theme::ColorType::CommandShortcutBackground);
					renderer.renderTextV(modifierArea, Theme::ColorType::CommandShortcut, keyName);
					left = modifierArea.x;
				}
			}
		}
	}
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

	for (auto command : mMainEditor.getChildCommands())
	{
		if (caseInsensitiveFind(command->context, mFilter) ||
			caseInsensitiveFind(command->name, mFilter))
			addItem(new CommandItem(*command));
	}

	sortItems(CommandItem::sort);
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


bool CommandSelector::CommandItem::sort(const CommandSelector::Item* ga, const CommandSelector::Item* gb)
{
	const auto& a = static_cast<const CommandItem&>(*ga);
	const auto& b = static_cast<const CommandItem&>(*gb);
	int cResult = strcmp(a.command.context, b.command.context);

	if (cResult == 0)
		return strcmp(a.command.name, b.command.name) < 0;

	return cResult < 0;
}


void CommandSelector::onModalStatusChange(bool isNowModal)
{
	// Make sure text field will receive SDL_TEXTINPUTs
	// and disables them after dialog close
	mFilterField->setIsEditing(isNowModal);

	// Reset filter on modal open
	strcpy(mFilter, "");
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
