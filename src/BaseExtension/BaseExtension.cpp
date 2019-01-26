#include "BaseExtension.h"
#include "../UIComponentFactory.h"
#include "WaveView.h"
#include "../Prototracker.h"

void BaseExtension::init(Prototracker& prototracker, IPlayer& player, const Song& song)
{
    mSynth = new Synth();
}


void BaseExtension::deinit()
{
    delete mSynth;
}


ISynth* BaseExtension::registerSynth()
{
    return mSynth;
}


void BaseExtension::registerUIComponents(UIComponentFactory& factory, EditorState& editorState)
{
    factory.registerComponent("WaveView", [&](const Theme::Element& element){ return new WaveView(editorState, mSynth->getWaveStore()); });
}
