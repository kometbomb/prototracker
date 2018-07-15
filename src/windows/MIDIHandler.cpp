#include "MIDIHandler.h"
#include "Debug.h"
#include "MainEditor.h"

MIDIHandler::MIDIHandler(MainEditor& mainEditor)
    : MIDIHandlerBase(mainEditor), mHandle(0), mDeviceId(-1), mIsRunning(false)
{
    debug("[MIDI] %d devices detected", midiInGetNumDevs());
}


MIDIHandler::~MIDIHandler()
{
}


bool MIDIHandler::registerCallback(const char *name)
{
    mIsRunning = false;

    for (int i = 0 ; i < midiInGetNumDevs() ; ++i)
    {
        MIDIINCAPS caps;
        if (midiInGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR && (name == NULL || strcmp(caps.szPname, name) == 0))
        {
            mDeviceId = i;

            if (midiInOpen(&mHandle, mDeviceId, reinterpret_cast<DWORD_PTR>(midiInProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION) != MMSYSERR_NOERROR)
            {
                debug("[MIDI] midiInOpen error");
                return false;
            }

            return true;
        }
    }

    return false;
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
        Uint8 status = dwParam1 & 255;
        Uint8 data1 = (dwParam1 >> 8) & 127, data2 = (dwParam1 >> 16) & 127;
        handler.onMessage(status, data1, data2, dwParam2);
    }
}


bool MIDIHandler::run(const char *name)
{
    if (mIsRunning)
        stop();

    mIsRunning = false;

    if (!registerCallback(name))
        return false;

    if (midiInStart(mHandle) == MMSYSERR_NOERROR)
    {
        mIsRunning = true;
        return true;
    }

    debug("[MIDI] midiInStart error");

    return false;
}


void MIDIHandler::stop()
{
    if (!mIsRunning)
        return;

    if (midiInStop(mHandle) != MMSYSERR_NOERROR)
    {
        debug("[MIDI] midiInStop error");
    }

    unregisterCallback();

    mIsRunning = false;
}


const char * MIDIHandler::getCurrentDeviceName() const
{
    static MIDIINCAPS caps;
    if (midiInGetDevCaps(mDeviceId, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
        return caps.szPname;

    return NULL;
}


int MIDIHandler::getCurrentDeviceID() const
{
    return mDeviceId;
}
