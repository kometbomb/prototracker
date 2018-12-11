#pragma once

struct ISynth;
struct Song;
struct UIComponentFactory;

class Extension
{
public:
  virtual ~Extension();
  virtual void init();
  virtual void deinit();
  virtual void registerUIComponents(UIComponentFactory& factory);
  virtual void registerFileSections(Song& song);
  virtual ISynth* registerSynth();
};
