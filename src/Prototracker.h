#pragma once

#include "SDL.h"
#include <string>
#include <map>
#include <typeinfo>
#include "Extension.h"

struct EditorState;
struct IPlayer;
struct Song;
struct ISynth;
struct Mixer;
struct MainEditor;
struct Gamepad;
struct Renderer;
struct UIComponentFactory;

class Prototracker {
	Renderer *mRenderer;
	EditorState *mEditorState;
	IPlayer *mPlayer;
	Song *mSong;
	ISynth *mSynth;
	Mixer *mMixer;
	MainEditor *mMainEditor;
	Gamepad *mGamepad;
	UIComponentFactory *mUIComponentFactory;

	std::map<std::string, Extension*> mExtensions;

	Uint32 mPreviousTick;
	bool mReady;

	void initEditor();
	bool initRenderer();

public:
	Prototracker();
	~Prototracker();

	/**
	 * Register extension/dependency.
	 */

	template<class T>
	T& loadExtension();

	bool init();
	void deinit();
	bool handleEvents();
	std::string getSongBase64() const;
};


template<class T>
T& Prototracker::loadExtension() {
	auto typeName = typeid(T).name();
	auto foundExtension = mExtensions.find(typeName);

	if (foundExtension != mExtensions.end())
	{
		return static_cast<T&>(*foundExtension->second);
	}

	auto extension = new T();
	mExtensions[typeName] = extension;
	return *extension;
}
