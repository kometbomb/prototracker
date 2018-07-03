#pragma once

// Basic stub for MIDI for systems with no support

#include "../MIDIHandlerBase.h"

class MIDIHandler: public MIDIHandlerBase {
public:
    MIDIHandler();
    virtual ~MIDIHandler();

    virtual void run();
    virtual void stop();
};
