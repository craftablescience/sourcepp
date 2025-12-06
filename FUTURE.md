### Future Projects

- VMF parser/writer (`vmfpp`)
- VMT parser (`vmtpp`)

### Future Improvements

- `bsppp`
  - Console paklump write support
  - Support parsing more lumps
  - Think about GoldSrc support
- `fspp`
  - Continue adding Source engine filesystem features
- `gamepp`
  - Add a function to create instances of a game rather than just finding existing ones
    - When creating an instance of the game, attaching a console might be easier, or enabling -condebug,
      which would then allow reading output from the console
  - Add a method to take a screenshot of the game and move the file to a user-specified location
- `kvpp`
  - Add read support for all DMX encodings understood by the library to exist
  - Add DMX write support
  - Add a KV3 parser/writer
- `mdlpp`
  - Add a save method to write the constituent parts of a StudioModel to disk
  - Add an SMD parser/writer
  - Make something to construct StudioModel objects from a given model file like obj/glTF?
  - Parse animations/sequences
- `sndpp`
  - WAV write support
  - XWV (Xbox, X360/PS3) write support
  - VDAT section read/write support
  - Conversion support between all formats
- `toolpp`
  - Perhaps add the ability to parse TeamSpen's additions to the FGD format?
- `vpkpp`
  - Cache file handles so they're not constantly getting closed/opened
  - ORE write support
  - PCK v3 read/write support
  - XZP write support
- `vtfpp`
  - Add a MKS parser/writer
