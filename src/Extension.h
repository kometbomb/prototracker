#pragma once

struct ISynth;
struct Song;
struct UIComponentFactory;
struct EditorState;

class Extension
{
public:
	virtual ~Extension();

	/**
	 * Extension should initialise everything in init()
	 */

	virtual void init();

	/**
	 * Extension should deinitialise everything in deinit()
	 */

	virtual void deinit();

	/**
	 * Extension will register any new UI components when this is called
	 */

	virtual void registerUIComponents(UIComponentFactory& factory, EditorState& editorState);

	/**
	 * Extension will register any Song FileSection listeners here
	 */

	virtual void registerSectionListeners(Song& song);

	/**
	 * Return either an instance of ISynth to register a synth or NULL for no synth
	 */
	virtual ISynth* registerSynth();
};
