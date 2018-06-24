#include "TooltipDisplayer.h"
#include "TooltipManager.h"
#include "Renderer.h"
#include "Color.h"

TooltipDisplayer::TooltipDisplayer(EditorState& editorState, TooltipManager& manager)
	: Editor(editorState, false), mTooltipManager(manager)
{
}


void TooltipDisplayer::onDraw(Renderer& renderer, const SDL_Rect& area)
{
	const TooltipManager::Tooltip *tooltip = mTooltipManager.getVisibleTooltip();

	if (tooltip)
	{
		/* Center text inside the display area
		 */

		SDL_Rect textArea = renderer.getTextRect(tooltip->text.c_str());
		SDL_Rect finalArea = area;
		finalArea.x = tooltip->area.x + tooltip->area.w / 2 - textArea.w / 2;
		finalArea.y = tooltip->area.y - textArea.h / 2;
		finalArea.w = textArea.w;
		finalArea.h = textArea.h;

		SDL_Rect frameArea;
		frameArea.x = finalArea.x - 2;
		frameArea.y = finalArea.y - 2;
		frameArea.w = finalArea.w + 4;
		frameArea.h = finalArea.h + 4;

		if (frameArea.w > 0 && frameArea.h > 0)
		{
			renderer.setClip(frameArea);
			renderer.clearRect(frameArea, renderer.getTheme().getColor(Theme::ColorType::ModalBackground));
			renderer.drawRect(frameArea, renderer.getTheme().getColor(Theme::ColorType::ModalBorder));

			renderer.renderText(finalArea, renderer.getTheme().getColor(Theme::ColorType::NormalText), tooltip->text.c_str());
		}
	}
}


void TooltipDisplayer::onUpdate(int ms)
{
	if (mTooltipManager.isDirty())
	{
		mTooltipManager.setDirty(false);
		this->setDirty(true);
	}
}
