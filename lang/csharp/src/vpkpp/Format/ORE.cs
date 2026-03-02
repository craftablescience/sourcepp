using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class ORE : PackFile
{
	protected ORE(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static ORE? Create(string path)
	{
		var handle = DLL.vpkpp_ore_create(path);
		return handle == nint.Zero ? null : new ORE(handle);
	}

	public new static ORE? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_ore_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new ORE(handle);
	}
}
