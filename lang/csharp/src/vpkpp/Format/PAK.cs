using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

// ReSharper disable once InconsistentNaming
public enum PAKType
{
	PAK,
	SIN,
	HROT,
}

public class PAK : PackFile
{
	protected PAK(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static PAK? Create(string path)
	{
		var handle = DLL.vpkpp_pak_create(path);
		return handle == nint.Zero ? null : new PAK(handle);
	}
	
	public static PAK? Create(string path, PAKType type)
	{
		var handle = DLL.vpkpp_pak_create_with_options(path, type);
		return handle == nint.Zero ? null : new PAK(handle);
	}

	public new static PAK? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_pak_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new PAK(handle);
	}

	public PAKType Type
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_pak_get_type(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_pak_set_type(Handle, value);
		}
	}
}
