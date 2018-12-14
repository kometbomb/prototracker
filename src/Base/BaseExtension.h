#pragma once

#include "../Extension.h"
#include "Synth.h"

struct IPlayer;

class BaseExtension: public Extension
{
    Synth *mSynth;
public:
    virtual void init(IPlayer& player, const Song& song);
    virtual void deinit();
    virtual ISynth* registerSynth();
  	virtual void registerUIComponents(UIComponentFactory& factory, EditorState& editorState);
};
