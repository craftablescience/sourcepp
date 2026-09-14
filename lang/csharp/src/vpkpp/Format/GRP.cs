using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class GRP : PackFile
{
	protected GRP(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static GRP? Create(string path)
	{
		var handle = DLL.vpkpp_grp_create(path);
		return handle == nint.Zero ? null : new GRP(handle);
	}

	public new static GRP? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_grp_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new GRP(handle);
	}
}
