using System;

namespace sourcepp.vpkpp.Format;

using EntryCallback = Action<string, Entry>;
using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

public class REZ : PackFile
{
	protected REZ(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public new static REZ? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? requestProperty = null)
	{
		var handle = DLL.vpkpp_rez_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new REZ(handle);
	}

	public string FileType
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_rez_get_file_type(Handle)).Read();
		}
	}

	public string UserTitle
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_rez_get_user_title(Handle)).Read();
		}
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_rez_get_version(Handle);
		}
	}
}
