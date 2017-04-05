#include "TrackState.h"
#include "PatternRow.h"
#include <math.h>

TrackState::TrackState()
	: enabled(true), wave(0), queuedWave(0), tick(0), macro(0), macroSpeed(1), macroRow(0)
{
	
}
