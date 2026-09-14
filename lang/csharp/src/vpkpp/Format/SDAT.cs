using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class SDAT : PackFile
{
	protected SDAT(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static SDAT? Create(string path)
	{
		var handle = DLL.vpkpp_sdat_create(path);
		return handle == nint.Zero ? null : new SDAT(handle);
	}

	public new static SDAT? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_sdat_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new SDAT(handle);
	}
}
