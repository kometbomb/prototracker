#include "PatchEditor.h"
#include "../Renderer.h"
#include "PatchManager.h"
#include "Patch.h"

PatchEditor::PatchEditor(EditorState& editorState, PatchManager& manager)
    : GenericList(editorState), mPatchManager(manager), mEditedPatch(NULL)
{
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
	renderer.renderTextV(area, color, "%*s %9d", -width, descriptor.getParam(paramItem.paramIndex).paramName, mEditedPatch->paramValue[paramItem.paramIndex]);
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
    auto& value = mEditedPatch->paramValue[selectedItem.paramIndex];
    value = std::max(paramDesc.minValue, std::min(paramDesc.maxValue, value + delta));
}


void PatchEditor::setPatch(Patch& patch)
{
    mEditedPatch = &patch;
}
