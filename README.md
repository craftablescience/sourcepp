# Source Pretty Parsers
Several modern C++20 libraries for sanely parsing Valve formats, rolled into one big repository.

Looking for more parsers in this vein? Try my other project which relies on this one,
[VPKEdit](https://github.com/craftablescience/VPKEdit)! It's a library too, not just a GUI.

### Supported Formats

| Library Name | Supports                                                                                                                                                                                    | Read | Write | Author(s)                                                                                      |
|--------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|:----:|:-----:|------------------------------------------------------------------------------------------------|
| dmxpp        | [DMX](https://developer.valvesoftware.com/wiki/DMX) Binary v1-5                                                                                                                             |  ✅   |   ❌   | [@craftablescience](https://github.com/craftablescience)                                       |
| fgdpp        | [FGD](https://developer.valvesoftware.com/wiki/FGD)<br>[TeamSpen's Unified FGD](https://github.com/TeamSpen210/HammerAddons/wiki/Unified-FGD)                                               |  ✅   |   ❌   | [@Trico-Everfire](https://github.com/Trico-Everfire)                                           |
| kvpp         | [KeyValues](https://developer.valvesoftware.com/wiki/KeyValues) v1[^1]                                                                                                                      |  ✅   |   ❌   | [@craftablescience](https://github.com/craftablescience)                                       |
| mdlpp        | [MDL](https://developer.valvesoftware.com/wiki/MDL_(Source)) v44-49[^2]<br>[VTX](https://developer.valvesoftware.com/wiki/VTX) v7<br>[VVD](https://developer.valvesoftware.com/wiki/VVD) v4 |  ✅   |   ❌   | [@craftablescience](https://github.com/craftablescience)                                       |
| vicepp       | [VICE](https://developer.valvesoftware.com/wiki/VICE) Encrypted Files                                                                                                                       |  ✅   |   ✅   | [@craftablescience](https://github.com/craftablescience)                                       |
| vmfpp        | [VMF](https://developer.valvesoftware.com/wiki/VMF_(Valve_Map_Format))                                                                                                                      |  ✅   |   ❌   | [@Galaco](https://github.com/Galaco), [@craftablescience](https://github.com/craftablescience) |
| vtfpp        | [VTF](https://developer.valvesoftware.com/wiki/VTF_(Valve_Texture_Format)) v7.0-7.6                                                                                                         |  ✅   |   ❌   | [@craftablescience](https://github.com/craftablescience)                                       |

[^1]: Many text-based formats in Source are close to (if not identical to) KeyValues v1, such as [VDF](https://developer.valvesoftware.com/wiki/VDF), [VMT](https://developer.valvesoftware.com/wiki/VMT), and [VMF](https://developer.valvesoftware.com/wiki/VMF_(Valve_Map_Format)).

[^2]: The MDL parser is not complete. It is usable in its current state, but it does not currently parse more complex components like animations. This parser is still in development.
