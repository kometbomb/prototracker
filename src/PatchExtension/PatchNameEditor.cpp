#include "PatchNameEditor.h"
#include "PatchManager.h"
#include "Patch.h"
#include "../EditorState.h"

PatchNameEditor::PatchNameEditor(EditorState& editorState, const PatchManager& patchManager)
    : TextEditor(editorState), mPatchManager(patchManager)
{
    setSolidBackground(false);
	mEditorState.patch.addListener(this);
    onListenableChange(NULL);
}


void PatchNameEditor::onListenableChange(Listenable *listenable)
{
    setBuffer(mPatchManager.getPatch(mEditorState.patch).patchName, Patch::patchNameLength);
}
