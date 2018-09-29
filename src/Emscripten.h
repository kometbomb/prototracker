#pragma once

/*

C-callbacks for Emscripten

*/

#include "SDL.h"

// We use SDL_Events to signal the app actions from the host webpage
enum {
	EVERYTHING_DONE = SDL_USEREVENT,
	EVERYTHING_READY,
	SONG_IMPORTED,
	EXPORT_SONG,
	PLAY_SONG,
	NEW_SONG
};

extern "C"
{

// Emscripten init routines
void emSyncFs();
void emSyncFsAndShutdown();
void emSyncFsAndStartup();

// The webpage has imported a song in the Emscripten file system (imported-song.song), tells the app to load the song
void emOnFileImported();

// Saves the song in the Emscripten file system and triggers the host webpage to open a file download
void emExportFile();

// Used by the host webpage to trigger actions when webpage buttons are clicked
void emNewSong();
void emPlaySong();

// Called by the app when ready and running (signals the host webpage we are ready to go)
void emAppReady();
void emAppShutdown();

// Returns the current song as Base64
const char * emRequestSong();

};
