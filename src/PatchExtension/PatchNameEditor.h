#include "../TextEditor.h"

struct PatchManager;

class PatchNameEditor: public TextEditor
{
    const PatchManager& mPatchManager;
public:
    PatchNameEditor(EditorState& editorState, const PatchManager& patchManager);

    virtual void onListenableChange(Listenable *listenable);
};
