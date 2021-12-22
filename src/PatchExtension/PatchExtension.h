#pragma once

#include "../Extension.h"

struct PatchManager;
struct Song;

class PatchExtension: public Extension
{
    PatchManager *mPatchManager;
    const Song *mSong;

public:
    virtual void init(Prototracker& prototracker, IPlayer& player, const Song& song);
    virtual void deinit();
    virtual void registerUIComponents(UIComponentFactory& factory, EditorState& editorState);
  	virtual void registerSectionListeners(Song& song);

    PatchManager& getPatchManager() const;
};
