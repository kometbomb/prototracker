#include "PatchEditor.h"
#include "../Renderer.h"
#include "PatchManager.h"
#include "Patch.h"

PatchEditor::PatchEditor(EditorState& editorState, PatchManager& manager)
    : GenericList(editorState), mPatchManager(manager)
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
	renderer.renderTextV(area, color, "%*s %9d", -width, descriptor.getParam(paramItem.paramIndex).paramName, 0);
}


PatchEditor::PatchParamItem::PatchParamItem(int index)
    : paramIndex(index)
{

}
