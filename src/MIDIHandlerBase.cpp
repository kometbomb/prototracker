#include "MIDIHandlerBase.h"
#include "Debug.h"
#include "../MainEditor.h"

MIDIHandlerBase::MIDIHandlerBase(MainEditor& mainEditor)
    : mMainEditor(mainEditor)
{
    mChannel = new MIDIChannel[numChannels];
}


MIDIHandlerBase::~MIDIHandlerBase()
{
    delete[] mChannel;
}


void MIDIHandlerBase::onMessage(Uint8 status, Uint8 data1, Uint8 data2, Uint32 timestamp)
{
    Uint8 event = status & 0xf0, channel = status & 15;

    switch (event)
    {
        case 0x80:
            onKeyStateChange(channel, data1, false);
            break;

        case 0x90:
            onKeyStateChange(channel, data1, true);
            break;

        case 0xb0:
            onControllerChange(channel, data1, data2);
            break;
    }
}


void MIDIHandlerBase::onControllerChange(int channel, int controller, Uint8 value)
{
    mMutex.lock();
    mChannel[channel].controllerValue[controller] = value;
    mMutex.unlock();
}


void MIDIHandlerBase::onKeyStateChange(int channel, int key, bool keyDown)
{
    mMainEditor.onExternalKeyStateChange(key, keyDown);
}


MIDIHandlerBase::MIDIChannel::MIDIChannel()
{
    for (auto& value : controllerValue)
    {
        value = 0;
    }
}


Uint8 MIDIHandlerBase::getControllerValue(int channel, int controller) const
{
    mMutex.lock();
    Uint8 value = mChannel[channel].controllerValue[controller];
    mMutex.unlock();

    return value;
}
