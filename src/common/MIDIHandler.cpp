#include "MIDIHandler.h"

MIDIHandler::MIDIHandler(MainEditor& mainEditor)
    : MIDIHandlerBase(mainEditor)
{
}


MIDIHandler::~MIDIHandler()
{
}


bool MIDIHandler::run(const char *device)
{

}


void MIDIHandler::stop()
{

}


const char * MIDIHandler::getCurrentDeviceName() const
{
    return NULL;
}


int MIDIHandler::getCurrentDeviceID() const
{
    return -1;
}
