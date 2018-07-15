#include "MIDIDeviceSelector.h"
#include "Renderer.h"
#include "Color.h"
#include "SDL.h"
#include "TextEditor.h"
#include "Label.h"
#include "MessageBox.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include "MIDIHandlerBase.h"

MIDIDeviceSelector::MIDIDeviceSelector(EditorState& editorState)
	: GenericSelector(editorState)
{

}


MIDIDeviceSelector::~MIDIDeviceSelector()
{
}


void MIDIDeviceSelector::accept(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, true);
}


void MIDIDeviceSelector::reject(bool isFinal)
{
	mParent->onFileSelectorEvent(*this, false);
}


void MIDIDeviceSelector::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
	const MIDIDeviceItem& deviceItem = static_cast<const MIDIDeviceItem&>(item);
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

	int width = area.w / renderer.getFontWidth() - 10;

	renderer.renderTextV(area, color, "%s", deviceItem.name.c_str());
}


void MIDIDeviceSelector::populate(const MIDIHandlerBase& midiHandler)
{
	invalidateParent();

	clearItems();

	int numDevices = midiHandler.getNumDevices();
	for (int index = 0 ; index < numDevices ; ++index)
		addItem(new MIDIDeviceItem(midiHandler.getDevice(index)));

	selectItem(0);
}


const char * MIDIDeviceSelector::getSelectedDevice() const
{
	return static_cast<const MIDIDeviceItem&>(getSelectedItem()).name.c_str();
}


MIDIDeviceSelector::MIDIDeviceItem::MIDIDeviceItem(const char *_name)
	: name(_name)
{
}
