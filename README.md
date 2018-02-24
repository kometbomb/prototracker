# Prototracker

Note: Prototracker is not the same thing as Prototracker-modular or Prototracker-OPLL. They have their own forks.

Prototracker is a multiplatform fakebit chiptune tracker. [Try the online version.](http://kometbomb.net/prototracker/)

The editor is a fairly normal tracker. The synth is an absolutely minimal single-oscillator synth (with 256 preset waveforms). Macros are used to create "instruments" and also some normal channel effects. Most keyboard shortcuts are the same as in Protracker. See the docs/ directory for help.

Supported platforms:
 * HTML5 (see the link above)
 * Windows (MinGW)
 * Linux
 * Mac (just use the Linux makefile)
 * Chip8 (runs great on a PocketCHIP)
 * Probably anything SDL2 supports
 
## Building

Simply do ```make mingw``` where "mingw" is your platform of choice. Do ```make``` to see the supported platforms. 

### Building for OSX

To install SDL/SDL2 on OSX(using brew), type brew install sdl2 sdl sdl_image sdl_mixer sdl_ttf portmidi. If you are getting the fatal error: 'SDL_image.h' file not found error, type apt install libsdl2-image-dev. You should then be able to run make linux.

The binary needs the files in the assets/ directory to work properly.

## Prototracker as a synth testbed

Prototracker was also created for testing audio/synth code (while having some minimal setup for a song structure), so it is quite easily extendable. Each of the channels runs an instance of ITrackState and IOscillator, and will simply extend IPlayer and ISynth to setup your own ITrackState (from IPlayer) and/or IOscillator (from ISynth). 

The TrackState, Oscillator and Synth classes included (see src/) implement a minimal wavetable synth that has two extra track effects (W and Q). Since it's all per track, you can have each channel use a different set of effects and oscillators (think something like the audio capabilities of the NES). Just init the tracks in IPlayer::IPlayer() and ISynth::ISynth().
