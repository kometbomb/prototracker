#pragma once

#include "../Extension.h"
#include "Synth.h"

class BaseExtension: public Extension
{
    Synth *mSynth;
public:
    virtual void init();
    virtual void deinit();
    virtual ISynth* registerSynth();
  	virtual void registerUIComponents(UIComponentFactory& factory, EditorState& editorState);
};
