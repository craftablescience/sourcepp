using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class PCK : PackFile
{
	protected PCK(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static PCK? Create(string path)
	{
		var handle = DLL.vpkpp_pck_create(path);
		return handle == nint.Zero ? null : new PCK(handle);
	}
	
	public static PCK? Create(string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion)
	{
		var handle = DLL.vpkpp_pck_create_with_options(path, version, godotMajorVersion, godotMinorVersion, godotPatchVersion);
		return handle == nint.Zero ? null : new PCK(handle);
	}

	public new static PCK? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_pck_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new PCK(handle);
	}
}
