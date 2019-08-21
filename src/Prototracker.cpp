#include "App.h"
#include "Prototracker.h"
#include "Mixer.h"
#include "Song.h"
#include "Synth.h"
#include "Player.h"
#include "EditorState.h"
#include "Gamepad.h"
#include "Renderer.h"
#include "MainEditor.h"
#include "Emscripten.h"
#include "Debug.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Prototracker::Prototracker()
	: mReady(false)
{

}

Prototracker::~Prototracker()
{
}

bool Prototracker::init()
{
	mEditorState = new EditorState();
	mSong = new Song();
	mPlayer = new Player(*mSong);
	mGamepad = new Gamepad();
	mSynth = new Synth();
	mMixer = new Mixer(*mPlayer, *mSynth);
	mRenderer = new Renderer();

	mMainEditor = new MainEditor(*mEditorState, *mPlayer, mPlayer->getPlayerState(), *mSong, *mSynth, *mMixer);

	if (!initRenderer()) {
		return false;
	}

#ifndef __EMSCRIPTEN__
  	initEditor();
#endif

  	return true;
}


void Prototracker::deinit()
{
	mMixer->stopThread();

	delete mMainEditor;
	delete mMixer;
	delete mPlayer;
	delete mSong;
	delete mEditorState;
	delete mGamepad;
	delete mRenderer;
	delete mSynth;
}

bool Prototracker::initRenderer()
{
	Theme theme;

	if (!theme.load("assets/elements"))
	{
		return false;
	}

	if (!mRenderer->setTheme(theme))
	{
		return false;
	}

	SDL_Rect area = {0, 0, theme.getWidth(), theme.getHeight()};
	mMainEditor->setArea(area);

	if (!mMainEditor->loadElements(theme))
	{
		return false;
	}

	mPreviousTick = SDL_GetTicks();
	return true;
}


void Prototracker::initEditor()
{
    // Emscripten needs an absolute path to filesystem root
#ifdef __EMSCRIPTEN__
	const char *gamepadPath = "/assets/gamecontrollerdb.txt";
	const char *songPath = "/assets/dub.song";
#else
	const char *gamepadPath = "assets/gamecontrollerdb.txt";
	const char *songPath = "assets/dub.song";
#endif

	mGamepad->loadDefinitions(gamepadPath);
	mGamepad->initControllers();
	mReady = true;

	if (!mMainEditor->loadState())
		mMainEditor->loadSong(songPath);

	// Try to use the device from the config or autodetect if not set

	mMainEditor->setAudioDevice(mEditorState->audioDevice.c_str());
}

/**
 * Returns true until quit is signaled
 */

bool Prototracker::handleEvents()
{
	bool done = false;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
#ifdef __EMSCRIPTEN__
		if (event.type == NEW_SONG)
		{
			mMainEditor->newSong();
		}
		else if (event.type == PLAY_SONG)
		{
			mMainEditor->togglePlayStop();
		}
		else if (event.type == SONG_IMPORTED)
		{
			mMainEditor->loadSong("/imported/imported-song.song");
		}
		else if (event.type == EXPORT_SONG)
		{
			mMainEditor->exportSong();
		}
		else if (event.type == EVERYTHING_READY)
		{
			initEditor();
			emAppReady();
		}
		else if (event.type == EVERYTHING_DONE)
		{
			debug("Received shutdown event");
			mMainEditor->saveState();
			done = true;
			emscripten_cancel_main_loop();
			mMixer->stopThread();
			emAppShutdown();
			exit(0);
		}
		else
#endif
		if (event.type == SDL_APP_WILLENTERBACKGROUND)
		{
			mMainEditor->saveState();
		}
		else if (event.type == SDL_QUIT || event.type == SDL_APP_TERMINATING)
		{
			done = true;
			mMainEditor->saveState();
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			mGamepad->translateAxisToDPad(event);
		}
		else if (event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEREMAPPED)
		{
			/* We just reinit all controllers instead of initializing a specific one */

#if SDL_VERSION_ATLEAST(2,0,4) && !defined(__EMSCRIPTEN__)
			if (event.type == SDL_CONTROLLERDEVICEADDED)
				mMainEditor->showMessageV(Editor::MessageInfo, "Plugged in %s", SDL_JoystickNameForIndex(event.cdevice.which));
			else
				mMainEditor->showMessageV(Editor::MessageInfo, "Unplugged %s", SDL_GameControllerName(SDL_GameControllerFromInstanceID(event.cdevice.which)));
#endif

			mGamepad->deinitControllers();
			mGamepad->initControllers();


		}
#if SDL_VERSION_ATLEAST(2,0,4)
#ifndef __EMSCRIPTEN__
		// TODO: Connection events mess up emscripten audio timing because this event happens
		// before actual ready state.
		else if (event.type == SDL_AUDIODEVICEADDED)
		{
			mMainEditor->showMessageV(Editor::MessageInfo, "%s connected.",
				SDL_GetAudioDeviceName(event.adevice.which, 0));

			if (!mMixer->getCurrentDeviceName())
			{
				mMainEditor->setAudioDevice(SDL_GetAudioDeviceName(event.adevice.which, 0));
			}
		}
		else if (event.type == SDL_AUDIODEVICEREMOVED)
		{
			if (mMixer->getCurrentDeviceID() == event.adevice.which)
			{
				mEditorState->audioDevice = "";
				mMainEditor->showMessageV(Editor::MessageInfo, "%s disconnected.",
				mMixer->getCurrentDeviceName());
				mMixer->stopThread();
			}
		}
#endif
#endif
		else
		{
			mPlayer->lock();
			mRenderer->scaleEventCoordinates(event);
			mMainEditor->onEvent(event);
			mPlayer->unlock();
		}
	}

	if (mReady)
	{
		mPlayer->lock();
		mMainEditor->syncPlayerState();
		mMainEditor->syncSongParameters(*mSong);

		Uint32 ticks = SDL_GetTicks() - mPreviousTick;

		if (ticks > 0)
		{
			mMainEditor->update(ticks);
			mPreviousTick += ticks;
		}

		if (mMainEditor->isDirty())
		{
			mPlayer->unlock();
			mRenderer->beginRendering();
			mMainEditor->draw(*mRenderer, mRenderer->getWindowArea());
#ifndef __EMSCRIPTEN__
			mRenderer->present();
#endif
		}
		else
		{
			mPlayer->unlock();
#ifndef __EMSCRIPTEN__
			SDL_Delay(2);
#endif
		}

#ifdef __EMSCRIPTEN__
		mRenderer->present();
#endif
	}

	return !done;
}


std::string Prototracker::getSongBase64() const
{
	return mMainEditor->getSongBase64();
}
