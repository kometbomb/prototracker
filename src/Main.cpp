#include "SDL.h"
#include "SDL_image.h"
#include "Renderer.h"
#include "MainEditor.h"
#include "Song.h"
#include "FileSection.h"
#include "Player.h"
#include "Synth.h"
#include "Mixer.h"
#include "EditorState.h"
#include "Emscripten.h"
#include "Theme.h"
#include "Context.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

Context* _context;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Context::Context()
	: ready(false), done(false), themeLoaded(false), song(), player(song), synth(), mixer(player, synth), editorState(), mainEditor(editorState, player, player.getPlayerState(), song, synth)
{
	Theme theme;
	
	if (!theme.load("assets/elements"))
	{
		return;
	}
		
	if (!renderer.setTheme(theme))
	{
		return;
	}
	
	if (!mainEditor.loadElements(theme))
	{
		return;
	}
	
	themeLoaded = true;
}


Context::~Context()
{
}


void infinityAndBeyond(void *ctx)
{
	Context& context = *static_cast<Context*>(ctx);
	SDL_Event event;
		
	while (SDL_PollEvent(&event))
	{
#ifdef __EMSCRIPTEN__
		if (event.type == NEW_SONG)
		{
			context.mainEditor.newSong();
		}
		else if (event.type == PLAY_SONG)
		{
			context.mainEditor.togglePlayStop();
		}
		else if (event.type == SONG_IMPORTED)
		{
			context.mainEditor.loadSong("/imported/imported-song.song");
		}
		else if (event.type == EXPORT_SONG)
		{
			context.mainEditor.exportSong();
		}
		else if (event.type == EVERYTHING_READY)
		{
			context.gamepad.loadDefinitions("/assets/gamecontrollerdb.txt");
			context.gamepad.initControllers();
			
			context.mainEditor.loadState();
			context.ready = true;
			context.mixer.runThread();
			emAppReady();
		}
		else if (event.type == EVERYTHING_DONE)
		{
			emscripten_cancel_main_loop();
			context.done = true;
		}
		else 
#endif
		if (event.type == SDL_APP_WILLENTERBACKGROUND)
		{
			context.mainEditor.saveState();
		}
		else if (event.type == SDL_QUIT || event.type == SDL_APP_TERMINATING)
		{
			context.done = true;
			context.mainEditor.saveState();
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			context.gamepad.translateAxisToDPad(event);
		}
		else if (event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEREMAPPED)
		{
			/* We just reinit all controllers instead of initializing a specific one */
			
			if (event.type == SDL_CONTROLLERDEVICEADDED)
				context.mainEditor.showMessageV(Editor::MessageInfo, "Plugged in %s", SDL_JoystickNameForIndex(event.cdevice.which));
			else 
				context.mainEditor.showMessageV(Editor::MessageInfo, "Unplugged %s", SDL_GameControllerName(SDL_GameControllerFromInstanceID(event.cdevice.which)));
			
			context.gamepad.deinitControllers();
			context.gamepad.initControllers();
			
			
		}
		else
		{
			context.player.lock();
			context.mainEditor.onEvent(event);
			context.player.unlock();
		}
	}
	
	if (context.ready)
	{
		context.player.lock();
		context.mainEditor.syncPlayerState();
		context.mainEditor.syncSongParameters(context.song);
		
		if (context.mainEditor.isDirty())
		{
			context.player.unlock();
			context.renderer.beginRendering();
			context.mainEditor.draw(context.renderer, context.renderer.getWindowArea());
#ifndef __EMSCRIPTEN__
			context.renderer.present();
#endif
		}
		else
		{
			context.player.unlock();
#ifndef __EMSCRIPTEN__
			SDL_Delay(2);
#endif
		}
	
#ifdef __EMSCRIPTEN__
		context.renderer.present();
#endif
	}
}


extern "C" int main(int argc, char **argv)
{
#ifdef __EMSCRIPTEN__
	SDL_SetHint(SDL_HINT_GRAB_KEYBOARD, "1");
#endif
	SDL_Init(SDL_INIT_EVERYTHING|SDL_INIT_NOPARACHUTE);
	atexit(SDL_Quit);
	
	IMG_Init(IMG_INIT_PNG);
	atexit(IMG_Quit);
	
	_context = new Context();
	
	Context& context = *_context;
	
	if (!context.themeLoaded)
	{
		// Theme was not loaded - exit
		return 1;
	}
	
#ifdef __EMSCRIPTEN__
	emSyncFsAndStartup();
	chdir("/persistent");
	
	if (!context.mainEditor.loadState())
		context.mainEditor.loadSong("/assets/dub.song");
	
	emscripten_set_main_loop_arg(infinityAndBeyond, &context, -1, 1);
	
#else
	context.gamepad.loadDefinitions("assets/gamecontrollerdb.txt");
	context.gamepad.initControllers();
	context.ready = true;
	
	if (!context.mainEditor.loadState())
		context.mainEditor.loadSong("assets/dub.song");
	
	if (!context.mixer.runThread())
		context.mainEditor.showMessage(Editor::MessageError, "Could not open audio device");
	
	while (!context.done)
	{
		infinityAndBeyond(&context);
	}
	
	context.mixer.stopThread();
	
#endif

	delete _context;
	
	return 0;
}
