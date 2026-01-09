<!--suppress HtmlDeprecatedAttribute -->
<div>
  <img align="left" width="128px" src="https://github.com/craftablescience/sourcepp/blob/main/branding/logo.png?raw=true" alt="The Source Pretty Parsers logo. A printer-esque device is scanning a page with hex codes and printing a picture of Cordon Freeman." />
  <h1>Source Pretty Parsers</h1>
</div>

<div>
  <a href="https://github.com/craftablescience/sourcepp/actions" target="_blank" rel="noreferrer"><img src="https://img.shields.io/github/actions/workflow/status/craftablescience/sourcepp/build.yml?label=Build&logo=github&logoColor=%23FFFFFF" alt="Build Status" /></a>
  <a href="https://github.com/craftablescience/sourcepp/blob/main/LICENSE" target="_blank" rel="noreferrer"><img src="https://img.shields.io/github/license/craftablescience/sourcepp?label=License&logo=libreofficewriter&logoColor=%23FFFFFF" alt="License" /></a>
  <a href="https://discord.gg/ASgHFkX" target="_blank" rel="noreferrer"><img src="https://img.shields.io/discord/678074864346857482?label=Discord&logo=Discord&logoColor=%23FFFFFF" alt="Discord" /></a>
  <a href="https://ko-fi.com/craftablescience" target="_blank" rel="noreferrer"><img src="https://img.shields.io/badge/donate-006dae?label=Ko-fi&logo=ko-fi&logoColor=%23FFFFFF&color=%23B238A1" alt="Ko-Fi" /></a>
</div>

Several modern C++20 libraries for sanely parsing Valve formats.

## Other Languages

<div>
  <a href="https://pypi.org/project/sourcepp" target="_blank" rel="noreferrer"><img alt="Version" src="https://img.shields.io/pypi/v/sourcepp?logo=python&logoColor=%23FFFFFF&label=PyPI%20Version" /></a>
  <a href="https://pypi.org/project/sourcepp" target="_blank" rel="noreferrer"><img src="https://img.shields.io/pypi/pyversions/sourcepp?logo=python&logoColor=%23FFFFFF&label=Python%20Versions" alt="Python Versions" /></a>
</div>

Wrappers for libraries considered complete exist for C, C#, and/or Python, depending on the library.

The Python wrappers can be found on PyPI in the [sourcepp](https://pypi.org/project/sourcepp) package.

## Included Libraries

<table>
  <tr>
    <th>Library</th>
    <th>Supports</th>
    <th>Read</th>
    <th>Write</th>
    <th>Bindings</th>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="1"><code>bsppp</code></td>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/BSP_(Source)" target="_blank" rel="noreferrer">BSP</a> v17-27
      <br> &bull; Console modifications
      <br> &bull; Left 4 Dead 2 modifications
      <br> &bull; <a href="https://stratasource.org" target="_blank" rel="noreferrer">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="1" align="center">Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="1"><code>fspp</code><sup>*</sup></td>
    <td>Source 1 filesystem accessor</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="1" align="center"></td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>gamepp</code></td>
    <td>Get Source engine instance window title/position/size</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="3" align="center">C<br>Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>Run commands in a Source engine instance remotely</td>
    <td align="center">❌</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="5"><code>kvpp</code></td>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/DMX" target="_blank" rel="noreferrer">DMX</a>
      <br> &bull; Legacy binary v1-2 encoding (<code>binary_vN</code>)
      <br> &bull; Legacy SFM v1-9 encoding (<code>sfm_vN</code>)
      <br> &bull; Binary v1-5, v9 encodings (<code>binary</code>, <code>binary_seqids</code>)
      <br> &bull; <a href="https://github.com/TeamSpen210/srctools" target="_blank" rel="noreferrer">srctools</a> encodings (<code>unicode_*</code>)
    </td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="5" align="center">Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/KeyValues" target="_blank" rel="noreferrer">KeyValues</a> v1 Binary</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/KeyValues" target="_blank" rel="noreferrer">KeyValues</a> v1 Text<sup>&dagger;</sup></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="5"><code>mdlpp</code><sup>*</sup></td>
    <td><a href="https://developer.valvesoftware.com/wiki/MDL_(Source)" target="_blank" rel="noreferrer">MDL</a> v44-49</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="5" align="center"></td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/VTX" target="_blank" rel="noreferrer">VTX</a> v7</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/VVD" target="_blank" rel="noreferrer">VVD</a> v4</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>sndpp</code><sup>*</sup></td>
    <td>WAV</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="3" align="center"></td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>XWV v0-1, v4</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="5"><code>steampp</code></td>
    <td>Find Steam install folder</td>
    <td align="center">✅</td>
    <td align="center">-</td>
    <td rowspan="5" align="center">C<br>Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>Find installed Steam games</td>
    <td align="center">✅</td>
    <td align="center">-</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>Find Steam game library assets</td>
    <td align="center">✅</td>
    <td align="center">-</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>toolpp</code></td>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/FGD" target="_blank" rel="noreferrer">FGD (Source 1)</a>
      <br> &bull; <a href="https://jack.hlfx.ru/en" target="_blank" rel="noreferrer">J.A.C.K.</a> modifications
      <br> &bull; <a href="https://ficool2.github.io/HammerPlusPlus-Website" target="_blank" rel="noreferrer">Hammer++</a> modifications
      <br> &bull; <a href="https://stratasource.org" target="_blank" rel="noreferrer">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="3" align="center">Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/Command_Sequences" target="_blank" rel="noreferrer">WC</a> (CmdSeq) v0.1-0.2
      <br> &bull; <a href="https://ficool2.github.io/HammerPlusPlus-Website" target="_blank" rel="noreferrer">Hammer++</a> modifications
      <br> &bull; <a href="https://stratasource.org" target="_blank" rel="noreferrer">Strata Source</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="3"><code>vcryptpp</code></td>
    <td><a href="https://developer.valvesoftware.com/wiki/VICE" target="_blank" rel="noreferrer">VICE</a> encrypted files</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
    <td rowspan="3" align="center">C<br>C#<br>Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/Vfont" target="_blank" rel="noreferrer">VFONT</a> encrypted fonts</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="33"><code>vpkpp</code></td>
    <td>007 v1.1, v1.3 (007 - Nightfire)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="33" align="center">C<br>C#<br>Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>APK (Fairy Tale Busters)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>FGP v2-3 (PS3, Orange Box)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>FPX v10 (Tactical Intervention)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/GCF_archive" target="_blank" rel="noreferrer">GCF</a> v6</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>GMA v1-3 (Garry's Mod)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>HOG (Descent)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>OL (Worldcraft Object Library)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>ORE (Narbacular Drop)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>
      <a href="https://quakewiki.org/wiki/.pak" target="_blank" rel="noreferrer">PAK</a> (Quake, WON Half-Life)
      <br> &bull; <a href="https://en.wikipedia.org/wiki/Sin_(video_game)" target="_blank" rel="noreferrer">SiN</a> modifications
      <br> &bull; <a href="https://store.steampowered.com/app/824600/HROT" target="_blank" rel="noreferrer">HROT</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://docs.godotengine.org/en/stable/tutorials/export/exporting_pcks.html" target="_blank" rel="noreferrer">PCK</a> v1-2 (Godot Engine)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/VPK" target="_blank" rel="noreferrer">VPK</a> pre-v1, v1-2, v54
      <br> &bull; <a href="https://www.counter-strike.net/cs2" target="_blank" rel="noreferrer">Counter-Strike: 2</a> modifications
      <br> &bull; <a href="https://clientmod.ru" target="_blank" rel="noreferrer">Counter-Strike: Source ClientMod</a> modifications
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>VPK (Vampire: The Masquerade - Bloodlines)</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>VPP v1-3 (Red Faction)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>WAD v3</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>XZP v6 (Xbox, Half-Life 2)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>
      ZIP
      <br> &bull; <a href="https://github.com/BEEmod/BEE2-items" target="_blank" rel="noreferrer">BEE_PACK</a> alias (BEE2.4 Package)
      <br> &bull; <a href="https://developer.valvesoftware.com/wiki/Bonus_Maps" target="_blank" rel="noreferrer">BMZ</a> alias (Source 1 Bonus Maps)
      <br> &bull; FPK alias (Tactical Intervention)
      <br> &bull; <a href="https://doomwiki.org/wiki/PK3" target="_blank" rel="noreferrer">PK3</a> alias (Quake III)
      <br> &bull; <a href="https://doomwiki.org/wiki/PK4" target="_blank" rel="noreferrer">PK4</a> alias (Quake IV, Doom 3)
      <br> &bull; PKZ alias (Quake II RTX)
      <br> &bull; XZP2 modifications (X360 & PS3, misc. Source 1 titles)
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td rowspan="37"><code>vtfpp</code></td>
    <td><a href="https://wiki.mozilla.org/APNG_Specification" target="_blank" rel="noreferrer">APNG</a></td>
    <td align="center">✅</td>
    <td align="center">❌</td>
    <td rowspan="37" align="center">C<br>Python</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/BMP_file_format" target="_blank" rel="noreferrer">BMP</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://openexr.com" target="_blank" rel="noreferrer">EXR</a> v1</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>FRAMES (PS3, Orange Box)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/GIF" target="_blank" rel="noreferrer">GIF</a></td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/RGBE_image_format" target="_blank" rel="noreferrer">HDR</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/JPEG" target="_blank" rel="noreferrer">JPEG</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>PIC</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/PNG" target="_blank" rel="noreferrer">PNG</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://netpbm.sourceforge.net/doc/pnm.html" target="_blank" rel="noreferrer">PNM</a> (PGM, PPM)</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/PPL" target="_blank" rel="noreferrer">PPL</a> v0</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://www.adobe.com/creativecloud/file-types/image/raster/psd-file.html" target="_blank" rel="noreferrer">PSD</a></td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://qoiformat.org" target="_blank" rel="noreferrer">QOI</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developer.valvesoftware.com/wiki/Animated_Particles" target="_blank" rel="noreferrer">SHT</a> v0-1</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://en.wikipedia.org/wiki/Truevision_TGA" target="_blank" rel="noreferrer">TGA</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>TTX (TTH, TTZ) v1.0</td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>VBF v3</td>
    <td align="center">✅</td>
    <td align="center">❌</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td>
      <a href="https://developer.valvesoftware.com/wiki/VTF_(Valve_Texture_Format)" target="_blank" rel="noreferrer">VTF</a> v7.0-7.6
      <br> &bull; <a href="https://stratasource.org" target="_blank" rel="noreferrer">Strata Source</a> modifications
      <br> &bull; <a href="https://developer.valvesoftware.com/wiki/Half-Life_2_(Xbox)/Modding_Guide" target="_blank" rel="noreferrer">XTF</a> v5.0 (Xbox, Half-Life 2)
      <br> &bull; <a href="https://developer.valvesoftware.com/wiki/VTFX_file_format" target="_blank" rel="noreferrer">VTFX</a> v8 (X360 &amp; PS3, Orange Box)
      <br> &bull; <a href="https://developer.valvesoftware.com/wiki/VTFX_file_format" target="_blank" rel="noreferrer">VTF3</a> v8 (PS3, Portal 2 &amp; CS:GO)
    </td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
  <tr><!-- empty row to disable GitHub striped bg color --></tr>
  <tr>
    <td><a href="https://developers.google.com/speed/webp" target="_blank" rel="noreferrer">WebP</a></td>
    <td align="center">✅</td>
    <td align="center">✅</td>
  </tr>
</table>

(\*) These libraries are incomplete and still in development. Their interfaces are unstable and will likely change in the future.
Libraries not starred should be considered stable, and their existing interfaces will not change much if at all. Note that wrappers
only exist for stable libraries.

(&dagger;) Many text-based formats in Source are close to (if not identical to) KeyValues v1, such as [VMT](https://developer.valvesoftware.com/wiki/VMT) and [VMF](https://developer.valvesoftware.com/wiki/VMF_(Valve_Map_Format)).

## Gallery

Anything using the `sourcepp` parser set that I know of, directly or indirectly.

These are only the tools and games using `sourcepp` that I know of. If you would like to be listed here, [email me](mailto:lauralewisdev@gmail.com) or [join my Discord server](https://discord.gg/ASgHFkX), I'd love to hear from you!

### Tools

- [fgptool](https://github.com/craftablescience/fgptool): A tool to crack the filepath hashes in The Orange Box PS3 file groups.
- [gimp-vtf](https://github.com/chev2/gimp-vtf): A GIMP plugin to load and save VTF files.
- [gm_addon_optimization_tricks](https://github.com/wrefgtzweve/gm_addon_optimization_tricks): A desktop tool to optimize Garry's Mod addons/maps.
- [GodotSource](https://github.com/craftablescience/godotsource): A work-in-progress set of bindings to connect the `sourcepp` libraries to Godot. Allows GDScript to work with the libraries, and allows Godot to directly load Source engine assets from a user project or from installed Source games.
- [MareTF](https://github.com/craftablescience/MareTF): An open source MIT-licensed CLI/GUI tool that can create, extract from, preview the contents of and write to every variant of VTF file. Replicates the functionality of Valve's `vtex.exe` and VTFEdit.
- [Myst IV: Revolution](https://github.com/tomysshadow/M4Revolution): Performs various fixes for the game Myst IV: Revelation.
- [PBR-2-Source](https://github.com/koerismo/PBR-2-Source): A Python-powered GUI for converting PBR materials into materials compatible with the Source engine.
- [QVTF++](https://github.com/craftablescience/qvtfpp): A QImageIO plugin to load VTF textures, based on panzi's QVTF plugin.
- [RectMaker](https://github.com/cplbradley/RectMaker): A freeware GUI tool that can create and modify `.rect` files used in Hammer++'s hotspotting algorithm.
- [reloaded2ps3](https://github.com/craftablescience/reloaded2ps3): Convert the PC version of Portal Reloaded to a playable PS3 game.
- [Verifier](https://github.com/StrataSource/verifier): A small program that can build an index of a game's files, and validate existing files based on that index. Similar to Steam's "Verify integrity of game files" option, but without overwriting any files.
- [VPKEdit](https://github.com/craftablescience/VPKEdit): An open source MIT-licensed CLI/GUI tool that can create, extract from, preview the contents of and write to several pack file formats. Replicates the functionality of Valve's `vpk.exe` and GCFScape.
  - [bsp-linux-fix](https://github.com/dresswithpockets/bsp-linux-fix): Patches maps which have improperly cased packed assets by repacking the assets, fixing an issue on Linux.
  - [CS2-EomVotesFix](https://github.com/Kitof/CS2-EomVotesFix): Fixes displaying workshop map names and thumbnails during end-of-match voting for LAN events.
  - [dham](https://github.com/Seraphli/dham): Modifies Dota 2 hero aliases based on a configuration file and packages the changes.
  - [Linux BSP Case Folding Workaround](https://github.com/scorpius2k1/linux-bsp-casefolding-workaround): A bash script designed to resolve issues with improperly cased packed map assets in Source engine games on Linux. Extracting the assets allows the game to find them properly.
  - [props_scaling_recompiler](https://github.com/Ambiabstract/props_scaling_recompiler): Allows converting `prop_scalable` into a static prop, effectively implementing static prop scaling outside CS:GO.
  - [rock:sail](https://github.com/Le0X8/rocksail): CS2 client-side tool to use skins for free (only visible to the user of the tool).
  - [vpk2wad_nd](https://github.com/p2r3/vpk2wad_nd): Converts textures in a VPK to a WAD that can be used by Narbacular Drop maps.
- [VTF Forge](https://github.com/Trico-Everfire/VTF-Forge): A modern multiplatform recreation of VTFEdit, using Qt.
- [VTF Thumbnailer](htps://github.com/craftablescience/vtf-thumbnailer): Adds previews for VTF files in your file explorer of choice on Windows and Linux.

### Games

<table>
  <tr>
    <td><a href="https://store.steampowered.com/app/440000/Portal_2_Community_Edition/" target="_blank" rel="noreferrer"><img width="250px" src="https://shared.fastly.steamstatic.com/store_item_assets/steam/apps/440000/header.jpg" alt="Portal 2: Community Edition"/></a></td>
    <td>
      <ul>
        <li>Local addon assets are packed with <code>sourcepp</code>.</li>
        <li>Verifier and VPKEdit are shipped with the game.</li>
      </ul>
    </td>
  </tr>
  <tr>
    <td><a href="https://store.steampowered.com/app/669270/Momentum_Mod/" target="_blank" rel="noreferrer"><img width="250px" src="https://shared.fastly.steamstatic.com/store_item_assets/steam/apps/669270/header.jpg" alt="Momentum Mod"/></a></td>
    <td>
      <ul>
        <li>Some bundled textures are created and/or compressed with MareTF.</li>
        <li>Some bundled assets are packed with VPKEdit.</li>
      </ul>
    </td>
  </tr>
  <tr>
    <td><a href="https://store.steampowered.com/app/2954780/Nightmare_House_The_Original_Mod/" target="_blank" rel="noreferrer"><img width="250px" src="https://shared.fastly.steamstatic.com/store_item_assets/steam/apps/2954780/header.jpg" alt="Nightmare House: The Original Mod"/></a></td>
    <td>
      <ul>
        <li>Game assets are packed with VPKEdit.</li>
      </ul>
    </td>
  </tr>
</table>

## Special Thanks

- `bsppp` partial library redesign, lump compression and game lump parsing/writing support contributed by [@Tholp](https://github.com/Tholp1).
- `kvpp`'s support for DMX srctools formats was contributed by [@TeamSpen210](https://github.com/TeamSpen210).
- `steampp` is based on the [SteamAppPathProvider](https://github.com/Trico-Everfire/SteamAppPathProvider) library by [@Trico Everfire](https://github.com/Trico-Everfire) and [Momentum Mod](https://momentum-mod.org) contributors.
- `vpkpp`'s 007 parser is based on [reverse-engineering work](https://raw.githubusercontent.com/SmileyAG/dumpster/refs/heads/src_jb007nightfirepc_alurazoe/file_format_analysis.txt) by Alhexx.
- `vpkpp`'s GCF parser was contributed by [@eepycats](https://github.com/eepycats) and [@ymgve](https://github.com/ymgve).
- `vpkpp`'s HOG parser was contributed by [@erysdren](https://github.com/erysdren).
- `vpkpp`'s OL parser is based on [reverse-engineering work](https://github.com/erysdren/scratch/blob/main/kaitai/worldcraft_ol.ksy) by [@erysdren](https://github.com/erysdren).
- `vpkpp`'s ORE parser is based on [reverse-engineering work](https://github.com/erysdren/narbacular-drop-tools) by [@erysdren](https://github.com/erysdren).
- `vpkpp`'s VPP parser was contributed by [@erysdren](https://github.com/erysdren).
- `vpkpp`'s WAD3 parser/writer was contributed by [@ozxybox](https://github.com/ozxybox).
- `vtfpp`'s NICE/Lanczos-3 resize filter support was contributed by [@koerismo](https://github.com/koerismo).
- `vtfpp`'s SHT parser/writer was contributed by [@Trico Everfire](https://github.com/Trico-Everfire).
- `vtfpp`'s initial VTF write support was loosely based on work by [@Trico Everfire](https://github.com/Trico-Everfire).
- `vtfpp`'s HDRI to cubemap conversion code is modified from the [HdriToCubemap](https://github.com/ivarout/HdriToCubemap) library by [@ivarout](https://github.com/ivarout).
