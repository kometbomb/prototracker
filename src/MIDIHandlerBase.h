#pragma once

#include "SDL.h"
#include "Lockable.h"

class MIDIHandlerBase {
    static const int numChannels = 16;
    mutable Lockable mMutex;
    struct MIDIChannel {
        Uint8 controllerValue[128];
        MIDIChannel();
    };

    MIDIChannel *mChannel;

protected:
    void onControllerChange(int channel, int controller, Uint8 value);
    void onMessage(Uint8 status, Uint8 data1, Uint8 data2, Uint32 timestamp);

public:
    MIDIHandlerBase();
    virtual ~MIDIHandlerBase();

    virtual void run() = 0;
    virtual void stop() = 0;

    Uint8 getControllerValue(int channel, int controller) const;
};
