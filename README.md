# Source Pretty Parsers
Several modern C++20 libraries for sanely parsing Valve formats, rolled into one big repository.

Looking for more parsers in this vein? Try my other project which relies on this one,
[VPKEdit](https://github.com/craftablescience/VPKEdit)! It's a library too, not just a GUI.

### Supported Formats

| Library Name  | Supports                                                                                                                                                                                                     | Read | Write | Author(s)                  |
|---------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----:|:-----:|----------------------------|
| dmxpp         | [DMX](https://developer.valvesoftware.com/wiki/DMX) Binary v1-5                                                                                                                                              |  ✅   |   ❌   | @craftablescience          |
| fgdpp         | &bull; [Valve FGD syntax](https://developer.valvesoftware.com/wiki/FGD)<br>&bull; [TeamSpen's Unified FGD syntax](https://github.com/TeamSpen210/HammerAddons/wiki/Unified-FGD)                              |  ✅   |   ❌   | @Trico-Everfire            |
| studiomodelpp | &bull; [MDL](https://developer.valvesoftware.com/wiki/MDL_(Source)) v44-49<br>&bull; [VTX](https://developer.valvesoftware.com/wiki/VTX) v7<br>&bull; [VVD](https://developer.valvesoftware.com/wiki/VVD) v4 |  ✅   |   ❌   | @craftablescience          |
| vmfpp         | Any [VMF](https://developer.valvesoftware.com/wiki/VMF_(Valve_Map_Format))                                                                                                                                   |  ✅   |   ❌   | @Galaco, @craftablescience |
