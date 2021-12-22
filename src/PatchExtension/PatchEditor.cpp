#include "PatchEditor.h"
#include "../Renderer.h"
#include "../EditorState.h"
#include "PatchManager.h"
#include "Patch.h"

PatchEditor::PatchEditor(EditorState& editorState, PatchManager& manager)
    : GenericList(editorState), mPatchManager(manager)
{
    editorState.patch.addListener(this);

    for (int i = 0 ; i < manager.getDescriptor().getNumParams() ; ++i)
    {
        addItem(new PatchParamItem(i));
    }
}


void PatchEditor::renderItem(Renderer& renderer, const SDL_Rect& area, const Item& item, bool isSelected)
{
    const PatchParamItem& paramItem = static_cast<const PatchParamItem&>(item);
	Theme::ColorType color = Theme::ColorType::NormalText;

	if (isSelected)
		color = Theme::ColorType::SelectedRow;

	renderer.clearRect(area, Theme::ColorType::ModalBackground);

	int width = area.w / renderer.getFontWidth() - 10;
    const PatchDescriptor& descriptor = mPatchManager.getDescriptor();
    auto editedPatch = mPatchManager.getPatch(mEditorState.patch);
	renderer.renderTextV(area, color, "%*s %9d", -width, descriptor.getParam(paramItem.paramIndex).paramName, editedPatch.paramValue[paramItem.paramIndex]);
}


PatchEditor::PatchParamItem::PatchParamItem(int index)
    : paramIndex(index)
{

}


bool PatchEditor::onEvent(SDL_Event& event)
{
    switch (event.type)
    {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_LEFT:
                    editParam(-1);
                    return true;

                case SDLK_RIGHT:
                    editParam(1);
                    return true;
            }
            break;
    }

    return GenericList::onEvent(event);
}


void PatchEditor::editParam(int delta)
{
    auto selectedItem = static_cast<const PatchParamItem&>(getSelectedItem());
    auto paramDesc = mPatchManager.getDescriptor().getParam(selectedItem.paramIndex);
    auto editedPatch = mPatchManager.getPatch(mEditorState.patch);
    auto& value = editedPatch.paramValue[selectedItem.paramIndex];
    value = std::max(paramDesc.minValue, std::min(paramDesc.maxValue, value + delta));
}


void PatchEditor::onRequestCommandRegistration()
{
    registerCommand("Patch", "Next patch", [this]() { mEditorState.patch = std::min(mEditorState.patch + 1, PatchManager::maxPatches - 1); }, SDLK_KP_PLUS, KMOD_SHIFT);
    registerCommand("Patch", "Previous patch", [this]() { mEditorState.patch = std::max(mEditorState.patch - 1, 0); }, SDLK_KP_MINUS, KMOD_SHIFT);
}
