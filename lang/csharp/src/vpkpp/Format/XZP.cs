using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class XZP : PackFile
{
	protected XZP(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public new static XZP? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_xzp_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new XZP(handle);
	}
}
