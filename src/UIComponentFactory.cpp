#include "UIComponentFactory.h"

void UIComponentFactory::registerComponent(const char *componentName, EditorCreatorFunc creatorFunc)
{
  mComponentPrototypes[componentName] = creatorFunc;
}


Editor* UIComponentFactory::createComponent(const char *componentName, const Theme::Element& element) const
{
  auto createComponent = mComponentPrototypes.find(componentName);

  if (createComponent == mComponentPrototypes.end())
  {
    return NULL;
  }

  return createComponent->second(element);
}
