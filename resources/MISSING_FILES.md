# Missing files

The following resource files were part of the original project but are not
included in this repository for legal reasons (unclear redistribution rights).
To build a complete plugin, supply your own files with the same names.

## Font

| File | Purpose |
|---|---|
| `resources/automat.ttf` | Custom UI font ("automat"). Still referenced by `Automat.xcodeproj`; place a replacement font with this name in `resources/` and Xcode will bundle it. |

## Samples

Loaded at runtime from the bundle's `samples` folder (see `Automat.cpp`) and
from the user's presets folder (`kAutomatSamples` in `AutomatVersion.h`).

| File | Purpose |
|---|---|
| `resources/samples/Loop.aif` | Factory sample "Loop" |
| `resources/samples/Noise.aif` | Factory sample "Noise" |
| `resources/samples/Strings.aif` | Factory sample "Strings" |
| `resources/samples/Voices.aif` | Factory sample "Voices" |
