#pragma once

#include "SDL.h"
#include <string>

struct EditorState;
struct IPlayer;
struct Song;
struct ISynth;
struct Mixer;
struct MainEditor;
struct Gamepad;
struct Renderer;

class Prototracker {
	Renderer *mRenderer;
	EditorState *mEditorState;
	IPlayer *mPlayer;
	Song *mSong;
	ISynth *mSynth;
	Mixer *mMixer;
	MainEditor *mMainEditor;
	Gamepad *mGamepad;

	Uint32 mPreviousTick;
	bool mReady;

	void initEditor();
	bool initRenderer();

public:
	Prototracker();
	~Prototracker();

	bool init();
	void deinit();
	bool handleEvents();
	std::string getSongBase64() const;
};
