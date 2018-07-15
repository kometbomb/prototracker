#include "IOscillator.h"
#include "TrackState.h"

IOscillator::IOscillator()
	: mSampleRate(0)
{
}


IOscillator::~IOscillator()
{
}


void IOscillator::handleTrackState(ITrackState& trackState)
{
}


void IOscillator::setSampleRate(int rate)
{
	mSampleRate = rate;
}


void IOscillator::setEnabled(bool state)
{
	mIsEnabled = state;
}
