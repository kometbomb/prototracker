#include "PatchExtension.h"
#include "../UIComponentFactory.h"
#include "PatchEditor.h"
#include "PatchManager.h"

void PatchExtension::init(Prototracker& prototracker, IPlayer& player, const Song& song)
{
    mPatchManager = new PatchManager();
}


void PatchExtension::deinit()
{
    delete mPatchManager;
}


void PatchExtension::registerUIComponents(UIComponentFactory& factory, EditorState& editorState)
{
    factory.registerComponent("PatchEditor", [&](const Theme::Element& element){ return new PatchEditor(editorState, *mPatchManager); });
}
