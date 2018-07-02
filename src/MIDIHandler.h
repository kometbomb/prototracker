#pragma once

#include "Lockable.h"
#include "SDL.h"
#include <windows.h>
#include <mmsystem.h>

class MIDIHandler {
    static const int numChannels = 16;

    HMIDIIN mHandle;
    UINT mDeviceId;
    mutable Lockable mMutex;

    struct MIDIChannel {
        Uint8 controllerValue[128];
        MIDIChannel();
    };

    MIDIChannel *mChannel;

    static void CALLBACK midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

    bool registerCallback();
    void unregisterCallback();

protected:
    void onMessage(Uint32 message, Uint32 timestamp);
    void onControllerChange(int channel, int controller, Uint8 value);

public:
    MIDIHandler();
    virtual ~MIDIHandler();

    virtual void run();
    virtual void stop();

    Uint8 getControllerValue(int channel, int controller) const;
};
