#include "Extension.h"
#include <stdlib.h>

Extension::~Extension()
{

}


void Extension::init(Prototracker& prototracker, IPlayer& player, const Song& song)
{

}


void Extension::deinit()
{

}


void Extension::registerUIComponents(UIComponentFactory& factory, EditorState& editorState)
{
}


void Extension::registerSectionListeners(Song& song)
{

}


ISynth* Extension::registerSynth()
{
    return NULL;
}
