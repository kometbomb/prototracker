#include "PatchExtension.h"
#include "../UIComponentFactory.h"
#include "../Song.h"
#include "PatchEditor.h"
#include "PatchInfo.h"
#include "PatchNameEditor.h"
#include "PatchManager.h"

void PatchExtension::init(Prototracker& prototracker, IPlayer& player, const Song& song)
{
    mPatchManager = new PatchManager();
    mSong = &song;
}


void PatchExtension::deinit()
{
    delete mPatchManager;
}


void PatchExtension::registerUIComponents(UIComponentFactory& factory, EditorState& editorState)
{
    factory.registerComponent("PatchEditor", [&](const Theme::Element& element){ return new PatchEditor(editorState, *mPatchManager); });
    factory.registerComponent("PatchInfo", [&](const Theme::Element& element){ return new PatchInfo(editorState); });
    factory.registerComponent("PatchName", [&](const Theme::Element& element){ return new PatchNameEditor(editorState, *mPatchManager); });
}


PatchManager& PatchExtension::getPatchManager() const
{
    return *mPatchManager;
}


void PatchExtension::registerSectionListeners(Song& song)
{
    song.addSectionListener("PTCH", mPatchManager, SectionListener::Load | SectionListener::Save);
}
