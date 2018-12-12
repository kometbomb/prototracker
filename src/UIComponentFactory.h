#pragma once

#include <functional>
#include <map>
#include <string>
#include "Theme.h"

struct Editor;

typedef std::function<Editor*(const Theme::Element& element)> EditorCreatorFunc;

class UIComponentFactory
{
  std::map<std::string, EditorCreatorFunc> mComponentPrototypes;
public:
  void registerComponent(const char *componentName, EditorCreatorFunc creatorFunc);
  Editor* createComponent(const char *componentName, const Theme::Element& element) const;
};
