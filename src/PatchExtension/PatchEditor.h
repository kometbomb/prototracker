#pragma once

#include "../GenericList.h"

struct EditorState;
struct PatchManager;
struct Patch;

class PatchEditor: public GenericList
{
    const PatchManager& mPatchManager;
    Patch* mEditedPatch;

    struct PatchParamItem: public Item
    {
        int paramIndex;

        PatchParamItem(int index);
    };

    void editParam(int delta);

protected:
    virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
public:
    PatchEditor(EditorState& editorState, PatchManager& patchManager);

    virtual bool onEvent(SDL_Event& event);
    void setPatch(Patch& patch);
};
