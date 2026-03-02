using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class FPX : PackFile
{
	protected FPX(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static FPX? Create(string path)
	{
		var handle = DLL.vpkpp_fpx_create(path);
		return handle == nint.Zero ? null : new FPX(handle);
	}

	public new static FPX? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_fpx_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new FPX(handle);
	}
}
