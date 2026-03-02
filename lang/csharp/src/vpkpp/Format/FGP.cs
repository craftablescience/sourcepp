using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class FGP : PackFile
{
	protected FGP(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static FGP? Create(string path)
	{
		var handle = DLL.vpkpp_fgp_create(path);
		return handle == nint.Zero ? null : new FGP(handle);
	}

	public new static FGP? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_fgp_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new FGP(handle);
	}

	public string LoadingScreenFilepath
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_fgp_get_loading_screen_file_path(Handle)).Read();
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_fgp_set_loading_screen_file_path(Handle, value);
		}
	}

	public static uint HashFilepath(string path)
	{
		return DLL.vpkpp_fgp_hash_file_path(path);
	}
}
