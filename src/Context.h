#pragma once

#include "MainEditor.h"
#include "Renderer.h"
#include "Mixer.h"
#include "Song.h"
#include "Player.h"
#include "Synth.h"
#include "EditorState.h"

struct Context
{
	bool ready;
	bool done;
	bool themeLoaded;
	
	Song song;
	Player player;
	Synth synth;
	
	EditorState editorState;
	MainEditor mainEditor;
	Renderer renderer;
	Mixer mixer;
	
	Context();
};
