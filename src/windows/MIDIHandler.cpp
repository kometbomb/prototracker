#include "MIDIHandler.h"
#include "Debug.h"
#include "MainEditor.h"

MIDIHandler::MIDIHandler(MainEditor& mainEditor)
    : MIDIHandlerBase(mainEditor), mHandle(0), mDeviceId(0)
{
    debug("[MIDI] %d devices detected", midiInGetNumDevs());
}


MIDIHandler::~MIDIHandler()
{
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
        Uint8 status = dwParam1 & 255;
        Uint8 data1 = (dwParam1 >> 8) & 127, data2 = (dwParam1 >> 16) & 127;
        handler.onMessage(status, data1, data2, dwParam2);
    }
}


void MIDIHandler::run()
{
    bool failed = false;

    if (registerCallback())
    {
        if (midiInStart(mHandle) != MMSYSERR_NOERROR)
        {
            debug("[MIDI] midiInStart error");
            failed = true;
        }
    }
    else
    {
        failed = true;
    }

    if (failed)
        mMainEditor.showMessage(Editor::MessageError, "Could not open MIDI input");
    else
        mMainEditor.showMessage(Editor::MessageInfo, "MIDI opened");
}


void MIDIHandler::stop()
{
    if (midiInStop(mHandle) != MMSYSERR_NOERROR)
    {
        debug("[MIDI] midiInStop error");
    }

    unregisterCallback();
}
