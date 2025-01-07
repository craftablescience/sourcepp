### Future Projects

- Source filesystem accessor (`fspp`)
- VMF parser/writer (`vmfpp`)
- VMT parser (`vmtpp`)

### Future Improvements

- External
  - Move from cryptopp to a maintained cryptography library
  - Move from compressonator to something that isn't a pile of garbage
  - Fork minizip-ng and change how it handles dependencies
- `bsppp`
  - Support parsing more lumps
  - Think about GoldSrc support
- `dmxpp`
  - Add support for v9+ DMX files
  - Add support for keyvalues2 and keyvalues2_flat formats
  - Add write support
- `gamepp`
  - Add a function to create instances of a game rather than just finding existing ones
    - When creating an instance of the game, attaching a console might be easier, or enabling -condebug,
      which would then allow reading output from the console
  - Add a method to take a screenshot of the game and move the file to a user-specified location
- `kvpp`
  - Reduce template spam
  - Move DMX/KV2 code here
  - Add a KV3 parser/writer
- `mdlpp`
  - Parse animations/sequences
  - Make something to construct StudioModel objects from a given model file like obj/glTF?
  - Add a save method to write the constituent parts of a StudioModel to disk
- `toolpp`
  - Perhaps add the ability to parse TeamSpen's additions to the FGD format?
- `vcryptpp`
  - Add VFONT write support
- `vpkpp`
  - Cache file handles so they're not constantly getting closed/opened
- `vtfpp`
  - Allow directly applying an HDRI to a VTF without requiring the user to write a custom wrapper
  - Add a MKS parser/writer
