# Source Pretty Parsers
Several modern C++20 libraries for sanely parsing Valve formats, rolled into one big repository.

Looking for more parsers in this vein? Try my other project which relies on this one,
[VPKEdit](https://github.com/craftablescience/VPKEdit)! It's a library too, not just a GUI.

## Supported Formats
This repository contains the following parsers:

### dmxpp
A parser for KV2/DMX files.

Currently supports:
- Binary
  - v1
  - v2
  - v3
  - v4
  - v5

### studiomodelpp
A parser for the various Source engine model formats.

Currently supports:
- MDL v44-v49
- VTX v7
- VVD v4

### vmfpp
A parser for uncompiled Source 1 map files. Supports any VMF.
