<div>
  <img align="left" width="101px" src="https://github.com/craftablescience/sourcepp/blob/main/branding/logo.png?raw=true" alt="The Source Pretty Parsers logo. A printer-esque device is scanning a page with hex codes and printing a picture of Cordon Freeman." />
  <h1>Source Pretty Parsers</h1>
</div>

![Build Status](https://img.shields.io/github/actions/workflow/status/craftablescience/sourcepp/build.yml?label=Build&logo=github&logoColor=%23FFFFFF)
![License](https://img.shields.io/github/license/craftablescience/sourcepp?label=License&logo=libreofficewriter&logoColor=%23FFFFFF)
![Discord](https://img.shields.io/discord/678074864346857482?label=Discord&logo=Discord&logoColor=%23FFFFFF)
![Ko-fi](https://img.shields.io/badge/donate-006dae?label=Ko-fi&logo=ko-fi)

Several modern C++20 libraries for sanely parsing Valve formats, rolled into one big repository.

## Included Libraries

<table>
  <tr>
    <th>Name</th>
    <th>Supports</th>
    <th>Read</th>
    <th>Write</th>
    <th>Wrappers</th>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="1"><code>bsppp</code><sup>*</sup></td>
    <td><a href="https://developer.valvesoftware.com/wiki/BSP_(Source)">BSP</a> v17-27</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="1" align="center"></td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="1"><code>dmxpp</code><sup>*</sup></td>
    <td><a href="https://developer.valvesoftware.com/wiki/DMX">DMX</a> Binary v1-5</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="1" align="center"></td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>gamepp</code></td>
    <td>Get Source engine instance window title/position/size</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="3" align="center">C<br>Python</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>Run commands in a Source engine instance remotely</td>
    <td align="center">❌</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="1"><code>kvpp</code></td>
    <td><a href="https://developer.valvesoftware.com/wiki/KeyValues">KeyValues</a> Text v1<sup>&dagger;</sup></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="1" align="center"></td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="5"><code>mdlpp</code><sup>*</sup></td>
    <td><a href="https://developer.valvesoftware.com/wiki/MDL_(Source)">MDL</a> v44-49</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="5" align="center"></td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/VTX">VTX</a> v7</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/VVD">VVD</a> v4</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>steampp</code></td>
    <td>Find Steam install folder</td>
    <td align="center">✅</td>
    <td align="center">-</td>
    <td rowspan="3" align="center">C<br>Python</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>Find installed Steam games</td>
    <td align="center">✅</td>
    <td align="center">-</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>toolpp</code></td>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/FGD">FGD (Source 1)</a>
      <br> &bull; <a href="https://ficool2.github.io/HammerPlusPlus-Website">Hammer++</a> modifications
      <br> &bull; <a href="https://stratasource.org">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="3" align="center"></td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/Command_Sequences">WC</a> (CmdSeq) v0.1-0.2
      <br> &bull; <a href="https://stratasource.org">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>vcryptpp</code></td>
    <td><a href="https://developer.valvesoftware.com/wiki/VICE">VICE</a> encrypted files</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="3" align="center">C<br>C#<br>Python</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/Vfont">VFONT</a> encrypted fonts</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="25"><code>vpkpp</code></td>
    <td><a href="https://developer.valvesoftware.com/wiki/Bonus_Maps">BMZ</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="25" align="center">C<br>C#</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/BSP_(Source)">BSP</a> v17-27</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>FPX v10 (Tactical Intervention)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/GCF_archive">GCF</a> v6</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>GMA v1-3 (Garry's Mod)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://quakewiki.org/wiki/.pak">PAK</a> (Quake, WON Half-Life)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://docs.godotengine.org/en/stable/tutorials/export/exporting_pcks.html">PCK</a> v1-2 (Godot Engine)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://doomwiki.org/wiki/PK3">PK3</a> (Quake III)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://doomwiki.org/wiki/PK4">PK4</a> (Quake IV, Doom 3)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/VPK">VPK</a> pre-v1, v1-2, v54
      <br> &bull; <a href="https://www.counter-strike.net/cs2">Counter-Strike: 2</a> modifications
      <br> &bull; <a href="https://clientmod.ru">Counter-Strike: Source ClientMod</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>VPK (Vampire: The Masquerade - Bloodlines)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>WAD v3</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>ZIP</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td rowspan="23"><code>vtfpp</code></td>
    <td><a href="https://en.wikipedia.org/wiki/BMP_file_format">BMP</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="23" align="center">Python</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://openexr.com">EXR</a> v1</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/GIF">GIF</a></td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/RGBE_image_format">HDR</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/JPEG">JPEG</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>PIC</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/PNG">PNG</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://netpbm.sourceforge.net/doc/pnm.html">PNM</a> (PGM, PPM)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/PPL">PPL</a> v0</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://www.adobe.com/creativecloud/file-types/image/raster/psd-file.html">PSD</a></td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/Truevision_TGA">TGA</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable github striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/VTF_(Valve_Texture_Format)">VTF</a> v7.0-7.6
      <br> &bull; <a href="https://stratasource.org">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
</table>

(\*) These libraries are incomplete and still in development. Their interfaces are unstable and will likely change in the future.
Libraries not starred should be considered stable, and their existing interfaces will not change much if at all. Note that wrappers
only exist for stable libraries.

(&dagger;) Many text-based formats in Source are close to (if not identical to) KeyValues v1, such as [VMT](https://developer.valvesoftware.com/wiki/VMT) and [VMF](https://developer.valvesoftware.com/wiki/VMF_(Valve_Map_Format)).

## Wrappers

Wrappers for libraries considered complete exist for C, C#, and/or Python, depending on the library. The Python wrappers can be
found on PyPI in the `sourcepp` package.

## Special Thanks

- `steampp` is based on the [SteamAppPathProvider](https://github.com/Trico-Everfire/SteamAppPathProvider) library by [@Trico Everfire](https://github.com/Trico-Everfire) and [Momentum Mod](https://momentum-mod.org) contributors.
- `vpkpp`'s GCF parser was contributed by [@bt](https://github.com/caatge) and [@ymgve](https://github.com/ymgve).
- `vpkpp`'s WAD3 parser was contributed by [@ozxybox](https://github.com/ozxybox).
- `vtfpp`'s write support is based on work by [@Trico Everfire](https://github.com/Trico-Everfire).

## Gallery

A list of projects using the `sourcepp` parser set. If you'd like to see your project here, make a PR!

- [VPKEdit](https://github.com/craftablescience/VPKEdit): An open source MIT-licensed tool that can create, extract from,
  preview the contents of and write to several pack file formats. A CLI application is bundled with the program which replicates
  the functionality of Valve's `vpk.exe`.
- [GodotSource](https://github.com/craftablescience/godotsource): A work-in-progress set of bindings to connect the `sourcepp`
  libraries to Godot. Allows GDScript to work with the libraries, and allows Godot to directly load Source engine assets from
  a user project or from installed Source games.
- [Verifier](https://github.com/StrataSource/verifier): A small program that can build an index of a game's files, and validate
  existing files based on that index. Think Steam's "Verify integrity of game files" option, but without actually overwriting any
  files. `sourcepp` is used to parse Steam depot configs, as well as enable indexing the contents of VPKs.
