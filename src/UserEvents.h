#pragma once

#include "SDL.h"

// We use SDL_Events to signal the app actions from the host webpage
// and to circumvent multiple event sources (multithreaded)

enum {
	EVERYTHING_DONE = SDL_USEREVENT,
	EVERYTHING_READY,
	SONG_IMPORTED,
	EXPORT_SONG,
	PLAY_SONG,
	NEW_SONG,
    MIDI_KEY_EVENT,
};
