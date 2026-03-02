using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class APK : PackFile
{
	protected APK(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static APK? Create(string path)
	{
		var handle = DLL.vpkpp_apk_create(path);
		return handle == nint.Zero ? null : new APK(handle);
	}

	public new static APK? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_apk_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new APK(handle);
	}
}
