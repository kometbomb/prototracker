#pragma once

#include "MainEditor.h"
#include "Renderer.h"
#include "Mixer.h"
#include "Song.h"
#include "Player.h"
#include "Synth.h"
#include "PatchManager.h"
#include "EditorState.h"
#include "Gamepad.h"

struct Context
{
	bool ready;
	bool done;
	bool themeLoaded;
	Uint32 previousTick;

	Song song;
	Player player;
	Synth synth;
    PatchManager patchManager;

	EditorState editorState;
	MainEditor mainEditor;
	Renderer renderer;
	Mixer mixer;
	Gamepad gamepad;

	Context();
	~Context();
};
