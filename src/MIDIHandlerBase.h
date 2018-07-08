#pragma once

#include "SDL.h"
#include "Lockable.h"

struct MainEditor;

class MIDIHandlerBase {
protected:
    static const int numChannels = 16;
    mutable Lockable mMutex;
    MainEditor& mMainEditor;

    struct MIDIChannel {
        Uint8 controllerValue[128];
        MIDIChannel();
    };

    MIDIChannel *mChannel;

protected:
    void onControllerChange(int channel, int controller, Uint8 value);
    void onKeyStateChange(int channel, int key, float velocity, bool keyDown);
    void onMessage(Uint8 status, Uint8 data1, Uint8 data2, Uint32 timestamp);

public:
    MIDIHandlerBase(MainEditor& mainEditor);
    virtual ~MIDIHandlerBase();

    virtual void run() = 0;
    virtual void stop() = 0;

    Uint8 getControllerValue(int channel, int controller) const;
};
