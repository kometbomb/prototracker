#pragma once

// Basic stub for MIDI for systems with no support

#include "../MIDIHandlerBase.h"

class MIDIHandler: public MIDIHandlerBase {
public:
    MIDIHandler(MainEditor& mainEditor);
    virtual ~MIDIHandler();

    virtual bool run(const char *deviceName);
    virtual void stop();
    virtual int getCurrentDeviceID() const;
    virtual const char * getCurrentDeviceName() const;
};
