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
- `gamepp`
  - Add a function to create instances of a game rather than just finding existing ones
    - When creating an instance of the game, attaching a console might be easier, or enabling -condebug,
      which would then allow reading output from the console
  - Add a method to take a screenshot of the game and move the file to a user-specified location
- `kvpp`
  - Add write support
- `mdlpp`
  - Parse animations/sequences
  - Make something to construct StudioModel objects from a given model file like obj/glTF?
  - Add a save method to write the constituent parts of a StudioModel to disk
- `vpkpp`
  - Read/write LZMA-compressed zip files, and make sure it works correctly as a BSP paklump
  - Cache file handles so they're not constantly getting closed/opened
- `vtfpp`
  - Improve conversion speeds between formats that use handrolled per-pixel converters (SIMD? Compute shader?)
  - Improve conversion between LDR/HDR
  - Add write support
