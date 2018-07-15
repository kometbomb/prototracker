#pragma once

#include "../MIDIHandlerBase.h"
#include <windows.h>
#include <mmsystem.h>

class MIDIHandler: public MIDIHandlerBase {
    HMIDIIN mHandle;
    UINT mDeviceId;
    bool mIsRunning;

    static void CALLBACK midiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    bool registerCallback(const char *name);
    void unregisterCallback();

public:
    MIDIHandler(MainEditor& mainEditor);
    virtual ~MIDIHandler();

    virtual bool run(const char *device);
    virtual void stop();
    virtual int getCurrentDeviceID() const;
    virtual const char * getCurrentDeviceName() const;
    virtual int getNumDevices() const;
    virtual const char * getDevice(int index) const;
};
