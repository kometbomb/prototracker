#pragma once

#include "Editor.h"

struct EditorState;
struct Renderer;
struct SDL_Rect;
struct Mixer;

#include "GenericSelector.h"

class AudioDeviceSelector: public GenericSelector
{

	struct AudioDeviceItem: public Item {
		std::string name;
		AudioDeviceItem(const char *name);
	};

	virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
	virtual void accept(bool isFinal = false);
	virtual void reject(bool isFinal = false);

public:
	AudioDeviceSelector(EditorState& editorState);
	virtual ~AudioDeviceSelector();

	/* Fill device list from Mixer
	 */
	void populate(const Mixer& mixer);


	/* After the dialog finishes this will return the path
	 * and filename to selected (or new) file
	 */
	const char * getSelectedDevice() const;
};
