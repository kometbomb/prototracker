#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct MIDIHandlerBase;

#include "GenericSelector.h"

class MIDIDeviceSelector: public GenericSelector
{

	struct MIDIDeviceItem: public Item {
		std::string name;
		MIDIDeviceItem(const char *name);
	};

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);

public:
	MIDIDeviceSelector(EditorState& editorState);
	virtual ~MIDIDeviceSelector();

	/* Fill device list from MIDIHandler
	 */
	void populate(const MIDIHandlerBase& midiHandler);


	/* After the dialog finishes this will return the device
	 */
	const char * getSelectedDevice() const;
};
