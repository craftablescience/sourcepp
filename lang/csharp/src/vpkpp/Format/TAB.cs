using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

// ReSharper disable once InconsistentNaming
public enum TABVersion
{
	JC1_LE,
	JC1_BE,
}

public class TAB : PackFile
{
	protected TAB(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static TAB? Create(string path)
	{
		var handle = DLL.vpkpp_tab_create(path);
		return handle == nint.Zero ? null : new TAB(handle);
	}

	public static TAB? Create(string path, TABVersion version, uint sectorSize)
	{
		var handle = DLL.vpkpp_tab_create_with_options(path, version, sectorSize);
		return handle == nint.Zero ? null : new TAB(handle);
	}

	public new static TAB? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_tab_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new TAB(handle);
	}

	public TABVersion Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_tab_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_tab_set_version(Handle, value);
		}
	}

	public uint SectorSize
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_tab_get_sector_size(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_tab_set_sector_size(Handle, value);
		}
	}

	public static uint HashFilepath(string path)
	{
		return DLL.vpkpp_tab_hash_file_path(path);
	}
}
