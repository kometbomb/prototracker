#pragma once

#include "GenericList.h"

struct EditorState;
struct PatchManager;

class PatchEditor: public GenericList
{
    const PatchManager& mPatchManager;

    struct PatchParamItem: public Item
    {
        int paramIndex;

        PatchParamItem(int index);
    };
protected:
    virtual void renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected);
public:
    PatchEditor(EditorState& editorState, PatchManager& patchManager);
};
