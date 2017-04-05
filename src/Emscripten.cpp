#include "Emscripten.h"
#include "SDL.h"
#include "Context.h"
#include <cstdlib>



#ifdef __EMSCRIPTEN__

extern Context* _context;

#include <emscripten.h>

void emDeinit();

extern "C" void emFilesSyncedShutdown()
{
	SDL_Event event;
	event.type = EVERYTHING_DONE;
	SDL_PushEvent(&event);
}


extern "C" void emFilesSyncedStartup()
{
	SDL_Event event;
	event.type = EVERYTHING_READY;
	SDL_PushEvent(&event);
}

void emSyncFsAndStartup()
{
	EM_ASM(
		console.log("Mounting filesystem");
		
		FS.mkdir('/persistent');
		FS.mkdir('/imported');
		FS.mount(IDBFS, {}, '/persistent');
		
		FS.syncfs(true, function(err) {
            assert(!err);
			console.log("Filesystem is loaded, starting up");
            Module.ccall('emFilesSyncedStartup', 'v');
		});
	);
}


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


const char * emOnBeforeUnload(int eventType, const void *reserved, void *userData)
{
	return "";
}


extern "C" void emOnFileImported()
{
	SDL_Event event;
	event.type = SONG_IMPORTED;
	SDL_PushEvent(&event);
}


extern "C" void emExportFile()
{
	SDL_Event event;
	event.type = EXPORT_SONG;
	SDL_PushEvent(&event);
}


extern "C" void emPlaySong()
{
	SDL_Event event;
	event.type = PLAY_SONG;
	SDL_PushEvent(&event);
}


extern "C" void emNewSong()
{
	SDL_Event event;
	event.type = NEW_SONG;
	SDL_PushEvent(&event);
}


extern "C" const char * emRequestSong()
{
	return _context->mainEditor.getSongBase64().c_str();
}
	
	
extern "C" void emAppReady()
{
	emscripten_run_script("appReady();");
}

#else
	
void emSyncFsAndShutdown() {}
void emSyncFsAndStartup() {}

#endif



