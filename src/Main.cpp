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
#include "Debug.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include "App.h"

Context* _context;

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Context::Context()
	: ready(false), done(false), themeLoaded(false), song(), player(song), synth(), mixer(player, synth), editorState(),
    patchManager(),
	mainEditor(editorState, player, player.getPlayerState(), song, synth, mixer, patchManager)
{
    mainEditor.setPatchManager(&patchManager);

    Theme theme;

	if (!theme.load("assets/elements"))
	{
		return;
	}

	if (!renderer.setTheme(theme))
	{
		return;
	}

	SDL_Rect area = {0, 0, theme.getWidth(), theme.getHeight()};
	mainEditor.setArea(area);

	if (!mainEditor.loadElements(theme))
	{
		return;
	}

	themeLoaded = true;
	previousTick = SDL_GetTicks();
}


Context::~Context()
{
}


void initEditor(Context& context)
{
	// Emscripten needs an absolute path to filesystem root
#ifdef __EMSCRIPTEN__
	const char *gamepadPath = "/assets/gamecontrollerdb.txt";
	const char *songPath = "/assets/dub.song";
#else
	const char *gamepadPath = "assets/gamecontrollerdb.txt";
	const char *songPath = "assets/dub.song";
#endif

	context.gamepad.loadDefinitions(gamepadPath);
	context.gamepad.initControllers();
	context.ready = true;

	if (!context.mainEditor.loadState())
		context.mainEditor.loadSong(songPath);

	// Try to use the device from the config or autodetect if not set

	context.mainEditor.setAudioDevice(context.editorState.audioDevice.c_str());
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
			initEditor(context);
			emAppReady();
		}
		else if (event.type == EVERYTHING_DONE)
		{
			debug("Received shutdown event");
			context.mainEditor.saveState();
			context.done = true;
			emscripten_cancel_main_loop();
			context.mixer.stopThread();
			delete &context;
			emAppShutdown();
			exit(0);
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

#if SDL_VERSION_ATLEAST(2,0,4) && !defined(__EMSCRIPTEN__)
			if (event.type == SDL_CONTROLLERDEVICEADDED)
				context.mainEditor.showMessageV(Editor::MessageInfo, "Plugged in %s", SDL_JoystickNameForIndex(event.cdevice.which));
			else
				context.mainEditor.showMessageV(Editor::MessageInfo, "Unplugged %s", SDL_GameControllerName(SDL_GameControllerFromInstanceID(event.cdevice.which)));
#endif

			context.gamepad.deinitControllers();
			context.gamepad.initControllers();


		}
#if SDL_VERSION_ATLEAST(2,0,4)
#ifndef __EMSCRIPTEN__
		// TODO: Connection events mess up emscripten audio timing because this event happens
		// before actual ready state.
		else if (event.type == SDL_AUDIODEVICEADDED)
		{
			context.mainEditor.showMessageV(Editor::MessageInfo, "%s connected.",
				SDL_GetAudioDeviceName(event.adevice.which, 0));

			if (!context.mixer.getCurrentDeviceName())
			{
				context.mainEditor.setAudioDevice(SDL_GetAudioDeviceName(event.adevice.which, 0));
			}
		}
		else if (event.type == SDL_AUDIODEVICEREMOVED)
		{
			if (context.mixer.getCurrentDeviceID() == event.adevice.which)
			{
				context.editorState.audioDevice = "";
				context.mainEditor.showMessageV(Editor::MessageInfo, "%s disconnected.",
				context.mixer.getCurrentDeviceName());
				context.mixer.stopThread();
			}
		}
#endif
#endif
		else
		{
			context.player.lock();
			context.renderer.scaleEventCoordinates(event);
			context.mainEditor.onEvent(event);
			context.player.unlock();
		}
	}

	if (context.ready)
	{
		context.player.lock();
		context.mainEditor.syncPlayerState();
		context.mainEditor.syncSongParameters(context.song);

		Uint32 ticks = SDL_GetTicks() - context.previousTick;

		if (ticks > 0)
		{
			context.mainEditor.update(ticks);
			context.previousTick += ticks;
		}

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
	emscripten_set_main_loop_arg(infinityAndBeyond, &context, -1, 1);

#else
	initEditor(context);

	while (!context.done)
	{
		infinityAndBeyond(&context);
	}
#endif

	context.mixer.stopThread();

	delete _context;

	debug("Exiting...");

	return 0;
}
