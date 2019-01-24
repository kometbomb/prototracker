#pragma once

#include "../Extension.h"

struct PatchManager;

class PatchExtension: public Extension
{
    PatchManager *mPatchManager;
public:
    virtual void init(Prototracker& prototracker, IPlayer& player, const Song& song);
    virtual void deinit();
    virtual void registerUIComponents(UIComponentFactory& factory, EditorState& editorState);

    PatchManager& getPatchManager() const;
};
