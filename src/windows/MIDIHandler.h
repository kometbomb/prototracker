#pragma once

#include "../MIDIHandlerBase.h"
#include <windows.h>
#include <mmsystem.h>

class MIDIHandler: public MIDIHandlerBase {
    HMIDIIN mHandle;
    UINT mDeviceId;

    static void CALLBACK midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    bool registerCallback();
    void unregisterCallback();

public:
    MIDIHandler();
    virtual ~MIDIHandler();

    virtual void run();
    virtual void stop();
};
