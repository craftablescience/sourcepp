using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class VPK_VTMB : PackFile
{
	protected VPK_VTMB(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static VPK_VTMB? Create(string path)
	{
		var handle = DLL.vpkpp_vpk_vtmb_create(path);
		return handle == nint.Zero ? null : new VPK_VTMB(handle);
	}

	public new static VPK_VTMB? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_vpk_vtmb_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new VPK_VTMB(handle);
	}
}
