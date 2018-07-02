#include "MIDIHandler.h"
#include "Debug.h"

MIDIHandler::MIDIHandler()
    : mHandle(0), mDeviceId(0)
{
    debug("[MIDI] %d devices detected", midiInGetNumDevs());

    mChannel = new MIDIChannel[numChannels];
}


MIDIHandler::~MIDIHandler()
{
    delete[] mChannel;
}


bool MIDIHandler::registerCallback()
{
    if (midiInOpen(&mHandle, mDeviceId, reinterpret_cast<DWORD_PTR>(midiInProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
    {
        debug("[MIDI] midiInOpen error");
        return false;
    }

    return true;
}


void MIDIHandler::unregisterCallback()
{
    if (midiInClose(mHandle) != MMSYSERR_NOERROR)
    {
        debug("[MIDI] midiInClose error");
    }
}


void CALLBACK MIDIHandler::midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    MIDIHandler& handler = *reinterpret_cast<MIDIHandler*>(dwInstance);

    if (wMsg == MIM_DATA)
    {
        handler.onMessage(dwParam1, dwParam2);
    }
}


void MIDIHandler::onMessage(Uint32 message, Uint32 timestamp)
{
    Uint8 event = message & 0xf0, channel = message & 15;
    Uint8 data1 = (message >> 8) & 127, data2 = (message >> 16) & 127;

    debug("[MIDI] [%08u] event = %x channel = %x, data1 = %02x data2 = %02x", timestamp, event, channel, data1, data2);

    switch (event)
    {
        case 0xb0:
            onControllerChange(channel, data1, data2);
            break;
    }
}


void MIDIHandler::run()
{
    if (registerCallback())
    {
        if (midiInStart(mHandle) != MMSYSERR_NOERROR)
        {
            debug("[MIDI] midiInStart error");
        }
    }
}


void MIDIHandler::stop()
{
    if (midiInStop(mHandle) != MMSYSERR_NOERROR)
    {
        debug("[MIDI] midiInStop error");
    }

    unregisterCallback();
}


void MIDIHandler::onControllerChange(int channel, int controller, Uint8 value)
{
    mMutex.lock();
    mChannel[channel].controllerValue[controller] = value;
    mMutex.unlock();
}


MIDIHandler::MIDIChannel::MIDIChannel()
{
    for (auto& value : controllerValue)
    {
        value = 0;
    }
}


Uint8 MIDIHandler::getControllerValue(int channel, int controller) const
{
    mMutex.lock();
    Uint8 value = mChannel[channel].controllerValue[controller];
    mMutex.unlock();

    return value;
}
