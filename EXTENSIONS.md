# Extensions

Prototracker can be extended (compile-time) with modular extensions. The exensions can provide new UI components ("Editors") and the main synth engine. The idea is that any forked trackers can pick and choose interesting features while avoiding feature bloat.

The basic Prototracker functionality is in `src/BaseExtension`, you should replace this with your own if you choose to fork Prototracker and name it descriptively.

## Dependencies

`YourExtension::init()` is the place to load new dependency extensions with `Prototracker::loadExtension<T>()` (the Prototracker instance is passed as the first argument). This ensures the extension is loaded and not duplicated (in case multiple extensions depend on the same extension) and returns a reference to the extension. It is up to you what you do with the reference, in most cases you would use it to request a property of the dependency to use in your extension.

The main `Makefile` needs to be updated to enable extensions. I.e. locate the following line and add your extension to it:

    ...

    EXTENSIONS=BaseExtension YourExtension

    ...

## Problems

While changes to the main Prototracker fork can be isolated to extensions, it may be necessary to edit e.g. the TrackState class (to add effects) and those changes will not be automatically included when reusing an extension in another fork. The extension mechanism will still be useful for many features.
