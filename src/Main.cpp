#include "SDL.h"
#include "SDL_image.h"
#include "Debug.h"
#include "Prototracker.h"
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include "Emscripten.h"
#include <emscripten.h>

Prototracker *g_prototracker = NULL;

#endif

void runLoop(void *prototracker)
{
    static_cast<Prototracker*>(prototracker)->handleEvents();
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

    Prototracker prototracker;

    if (!prototracker.init())
    {
        return 1;
    }

#ifdef __EMSCRIPTEN__
    g_prototracker = &prototracker;

	emSyncFsAndStartup();
	emscripten_set_main_loop_arg(runLoop, &prototracker, -1, 1);

#else

	while (prototracker.handleEvents())
	{
        // Spin around
	}
#endif

	prototracker.deinit();

	debug("Exiting...");

	return 0;
}
