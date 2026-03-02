using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class WAD3 : PackFile
{
	protected WAD3(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static WAD3? Create(string path)
	{
		var handle = DLL.vpkpp_wad3_create(path);
		return handle == nint.Zero ? null : new WAD3(handle);
	}

	public new static WAD3? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_wad3_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new WAD3(handle);
	}
}
