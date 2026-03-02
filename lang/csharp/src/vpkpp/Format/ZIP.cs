using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class ZIP : PackFile
{
	protected ZIP(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static ZIP? Create(string path)
	{
		var handle = DLL.vpkpp_zip_create(path);
		return handle == nint.Zero ? null : new ZIP(handle);
	}

	public new static ZIP? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_zip_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new ZIP(handle);
	}

	public EntryCompressionType EntryCompressionType(string path)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_zip_get_entry_compression_type(Handle, path);
	}
	
	public void EntryCompressionType(string path, EntryCompressionType type)
	{
		ThrowIfDisposed();
		DLL.vpkpp_zip_set_entry_compression_type(Handle, path, type);
	}
	
	public short EntryCompressionStrength(string path)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_zip_get_entry_compression_strength(Handle, path);
	}
	
	public void EntryCompressionStrength(string path, short strength)
	{
		ThrowIfDisposed();
		DLL.vpkpp_zip_set_entry_compression_strength(Handle, path, strength);
	}
}
