### Future Projects

- Source filesystem accessor (`fspp`)
- VMF parser/writer (`vmfpp`)
- VMT parser (`vmtpp`)

### Future Improvements

- `bsppp`
  - Read/write LZMA-compressed lumps
- `dmxpp`
  - Add support for v9+ DMX files
  - Add support for keyvalues2 and keyvalues2_flat formats
  - Add write support
- `fgdpp`
  - Perhaps add the ability to parse TeamSpen's additions to the format?
- `kvpp`
  - Add write support
- `mdlpp`
  - Parse animations/sequences
  - Make something to construct StudioModel objects from a given model file like obj/glTF?
  - Add a save method to write the constituent parts of a StudioModel to disk
- `vpkpp`
  - Read/write LZMA-compressed zip files, and make sure it works correctly as a BSP paklump
  - Look into better ways to store archive entries, such as a trie ~~or a set~~
    - And in that vein, get rid of the `path` property on an entry
  - In fact, perhaps restructure the API so having a copy of the file metadata is unnecessary to access content!
- `vtfpp`
  - Improve conversion speeds between formats that use handrolled per-pixel converters (SIMD? Compute shader?)
  - Improve conversion between LDR/HDR
  - Add write support
