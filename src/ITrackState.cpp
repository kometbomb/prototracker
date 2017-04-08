#include "ITrackState.h"
#include "EffectParam.h"

ITrackState::ITrackState()
	: enabled(true), tick(0), macro(0), macroSpeed(1), macroRow(0)
{

}


ITrackState::~ITrackState()
{
}


bool ITrackState::handleEffectZeroTick(const EffectParam& effect, PlayerState& playerState)
{
	int asByte = effect.getParamsAsByte();
	
	switch (effect.effect)
	{
		case 'j':
			macroRow = asByte;
			return true;
			
		case 'm':
			macro = asByte;
			macroRow = 0;
			break;
			
		case 's':
			macroSpeed = asByte;
			break;
	}
	
	return false;
}


void ITrackState::handleEffectAnyTick(const EffectParam& effect, PlayerState& playerState)
{
	
}

