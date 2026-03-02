using System;

namespace sourcepp.vpkpp;

public class Entry : sourcepp.ManagedNativeHandle
{
	internal Entry(nint handle, bool managed = true) : base(handle, DLL.vpkpp_entry_close, managed)
	{
	}

	public uint Flags
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_flags(Handle);
		}
	}

	public uint ArchiveIndex
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_archive_index(Handle);
		}
	}

	public ulong Length
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_length(Handle);
		}
	}

	public ulong CompressedLength
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_compressed_length(Handle);
		}
	}

	public ulong Offset
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_offset(Handle);
		}
	}

	public byte[] ExtraData
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.Buffer(DLL.vpkpp_entry_get_extra_data(Handle)).Read<byte>();
		}
	}

	public uint Crc32
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_entry_get_crc32(Handle);
		}
	}

	public bool Unbaked
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_entry_is_unbaked(Handle));
		}
	}
}
