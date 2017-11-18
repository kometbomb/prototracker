#include "PlayerState.h"

PlayerState::PlayerState()
	: mode(Stop), songSpeed(6), songRate(50), sequenceRow(0), patternRow(0), cpuUse(0), tick(0), updateMask(-1)
{

}

int PlayerState::getUpdated()
{
	return updateMask;
}


void PlayerState::ackUpdated(int mask)
{
	updateMask &= ~mask;
}


void PlayerState::setUpdated(int mask)
{
	updateMask |= mask;
}


bool PlayerState::isPlaying() const
{
	if (mode == Play || mode == PlaySequenceRow)
		return true;

	return false;
}


bool PlayerState::shouldAdvanceSequenceRow() const
{
	if (mode == PlaySequenceRow)
		return false;

	return true;
}
