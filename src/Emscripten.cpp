#include "Emscripten.h"
#include "SDL.h"
#include <cstdlib>
#include "App.h"

#ifdef __EMSCRIPTEN__

#include "Prototracker.h"

extern Prototracker *g_prototracker;

#include <emscripten.h>

extern "C" {

void emDeinit();

EMSCRIPTEN_KEEPALIVE
void emFilesSyncedShutdown()
{
	SDL_Event event;
	event.type = EVERYTHING_DONE;
	SDL_PushEvent(&event);
}


EMSCRIPTEN_KEEPALIVE
void emFilesSyncedStartup()
{
	SDL_Event event;
	event.type = EVERYTHING_READY;
	SDL_PushEvent(&event);
}

EMSCRIPTEN_KEEPALIVE
void emSyncFsAndStartup()
{
	EM_ASM_({
		var appName = UTF8ToString($0);
		console.log("Mounting filesystem");
		var path = '/libsdl';
		FS.mkdir(path);
		path+='/'+appName;
		FS.mkdir(path);
		path+='/'+appName;
		FS.mkdir(path);
		FS.mkdir('/imported');
		FS.mount(IDBFS, {}, path);

		FS.syncfs(true, function(err) {
            assert(!err);
			console.log("Filesystem is loaded, starting up");
            Module.ccall('emFilesSyncedStartup', 'v');
		});
	}, APP_NAME);
}


EMSCRIPTEN_KEEPALIVE
void emSyncFs()
{
	EM_ASM(
		console.log("Syncing filesystem");

		FS.syncfs(false, function(err) {
            assert(!err);
			console.log("Filesystem synced");
		});
	);
}


EMSCRIPTEN_KEEPALIVE
void emSyncFsAndShutdown()
{
	EM_ASM(
		console.log("Saving filesystem");
		FS.syncfs(function(err) {
            assert(!err);
			console.log("Filesystem is saved, shutting down");
            Module.ccall('emFilesSyncedShutdown', 'v');
		});
	);
}


EMSCRIPTEN_KEEPALIVE
const char * emOnBeforeUnload(int eventType, const void *reserved, void *userData)
{
	return "";
}


EMSCRIPTEN_KEEPALIVE
void emOnFileImported()
{
	SDL_Event event;
	event.type = SONG_IMPORTED;
	SDL_PushEvent(&event);
}


EMSCRIPTEN_KEEPALIVE
void emExportFile()
{
	SDL_Event event;
	event.type = EXPORT_SONG;
	SDL_PushEvent(&event);
}


EMSCRIPTEN_KEEPALIVE
void emPlaySong()
{
	SDL_Event event;
	event.type = PLAY_SONG;
	SDL_PushEvent(&event);
}


EMSCRIPTEN_KEEPALIVE
void emNewSong()
{
	SDL_Event event;
	event.type = NEW_SONG;
	SDL_PushEvent(&event);
}


EMSCRIPTEN_KEEPALIVE
const char * emRequestSong()
{
	return g_prototracker->getSongBase64().c_str();
}


EMSCRIPTEN_KEEPALIVE
void emAppReady()
{
	emscripten_run_script("if (typeof appReady === \"function\") appReady();");
}

EMSCRIPTEN_KEEPALIVE
void emAppShutdown()
{
	emscripten_run_script("if (typeof appShutdown === \"function\") appShutdown();");
}

}

#else

void emSyncFsAndShutdown() {}
void emSyncFsAndStartup() {}

#endif
