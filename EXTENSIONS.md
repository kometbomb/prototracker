# Extensions

Prototracker can be extended (compile-time) with modular extensions. The exensions can provide new UI components ("Editors") and the main synth engine. The idea is that any forked trackers can pick and choose interesting features while avoiding feature bloat.

The basic Prototracker functionality is in `src/BaseExtension`, you should replace this with your own if you choose to fork Prototracker and name it descriptively.

The main `Makefile` needs to be updated to enable extensions. I.e. locate the following line and add your extension to it:

    ...

    EXTENSIONS=BaseExtension YourExtension

    ...

## Extension lifecycle

  1. `Extension::Extension()` will be called when a new extension is loaded by `Prototracker::loadExtension<T>()` and if it was not already constructed by earlier loads.
  2. `Extension::init()` will be called some time after `Prototracker::loadExtension<Extension>()` has been called. If you add a dependency in `Extension::init()`, it is ensured that it has been constructed but it is not ensured the init method for the extension has been called. You should only store the reference to the dependency here and use it later in the other lifecycle methods.
  3. `Extension::registerSynth()` will be called if an extension hasn't registered a synth yet - not a problem as only one extension should provide a synth.
  4. `Extension::registerUIComponents()` will be called.
  5. `Extension::registerSectionListeners()` will be called.
  6. `Extension::deinit()` (and `Extension::~Extension()`) will be called when Prototracker is shut down.

## Dependencies

`Extension::init()` is the place to load new dependency extensions with `Prototracker::loadExtension<T>()` (the Prototracker instance is passed as the first argument to `Extension::init()`). This makes sure the dependency is loaded and not duplicated (in case multiple extensions depend on the same extension) and returns a reference to the extension. Please take care of the fact the dependency has been loaded but is not necessarily initialized at this point.

## Problems

While many changes to the main Prototracker fork can be isolated to extensions, it may be necessary to edit e.g. the TrackState class (to add effects) and those changes will not be automatically included when reusing an extension in another fork. The extension mechanism will still be useful for many features.
