### Future Projects

- VMF parser/writer (`vmfpp`)
- VMT parser (`vmtpp`)

### Future Improvements

- External
  - Strip down cryptopp and stop downloading it at CMake configuration time
  - Move from compressonator to something that isn't a pile of garbage
    - There is literally nothing else that supports BC6H
  - Fork minizip-ng and change how it handles dependencies
- `bsppp`
  - Support parsing more lumps
  - Think about GoldSrc support
  - Console paklump support
- `dmxpp`
  - Add support for v9+ DMX files
  - Add support for keyvalues2 and keyvalues2_flat formats
  - Add write support
- `fspp`
  - Continue adding Source engine filesystem features
- `gamepp`
  - Add a function to create instances of a game rather than just finding existing ones
    - When creating an instance of the game, attaching a console might be easier, or enabling -condebug,
      which would then allow reading output from the console
  - Add a method to take a screenshot of the game and move the file to a user-specified location
- `kvpp`
  - Move DMX/KV2 code here
  - Add a KV3 parser/writer
- `mdlpp`
  - Parse animations/sequences
  - Make something to construct StudioModel objects from a given model file like obj/glTF?
  - Add a save method to write the constituent parts of a StudioModel to disk
  - Add an SMD parser/writer
- `sndpp`
  - WAV write support
  - XWV write support
- `toolpp`
  - Perhaps add the ability to parse TeamSpen's additions to the FGD format?
- `vcryptpp`
  - Add VFONT write support
- `vpkpp`
  - Cache file handles so they're not constantly getting closed/opened
- `vtfpp`
  - Add DPID resize filter
  - Allow directly applying an HDRI to a VTF without requiring the user to write a custom wrapper
  - Add a MKS parser/writer
