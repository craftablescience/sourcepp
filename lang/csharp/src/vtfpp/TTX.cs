using System;

namespace sourcepp.vtfpp;

public sealed class TTX : sourcepp.ManagedNativeHandle
{
	internal TTX(nint handle) : base(handle, DLL.vtfpp_ttx_close)
	{
	}

	public TTX(VTF vtf) : this(DLL.vtfpp_ttx_create(vtf.Handle))
	{
	}

	public TTX(ReadOnlySpan<byte> tthData, ReadOnlySpan<byte> ttzData = new()) : this(DLL.vtfpp_ttx_open_from_mem(tthData, (ulong) tthData.Length, ttzData, (ulong) ttzData.Length))
	{
	}

	public TTX(string tthPath, string ttzPath = "") : this(DLL.vtfpp_ttx_open_from_file(tthPath, ttzPath))
	{
	}

	public static implicit operator bool(TTX ttx)
	{
		ttx.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ttx_is_valid(ttx.Handle));
	}

	public byte VersionMajor
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ttx_get_major_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_major_version(Handle, value);
		}
	}
	
	public byte VersionMinor
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ttx_get_minor_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_minor_version(Handle, value);
		}
	}

	public (byte, byte) Version
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.vtfpp_ttx_get_major_version(Handle), DLL.vtfpp_ttx_get_minor_version(Handle));
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_version(Handle, value.Item1, value.Item2);
		}
	}

	public byte AspectRatioType
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ttx_get_aspect_ratio_type(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_aspect_ratio_type(Handle, value);
		}
	}

	public ulong[] MipFlags
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.Buffer(DLL.vtfpp_ttx_get_mip_flags(Handle)).Read<ulong>();
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_mip_flags(Handle, value, (ulong) value.Length);
		}
	}

	public VTF Vtf
	{
		get
		{
			ThrowIfDisposed();
			return new VTF(DLL.vtfpp_ttx_get_vtf(Handle), false);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_vtf(Handle, value.Handle);
		}
	}

	public short CompressionLevel
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ttx_get_compression_level(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ttx_set_compression_level(Handle, value);
		}
	}

	public (byte[], byte[]) Bake()
	{
		ThrowIfDisposed();
		ulong tthLen = 0, ttzLen = 0;
		var ttxData = new sourcepp.Buffer(DLL.vtfpp_ttx_bake(Handle, ref tthLen, ref ttzLen)).Read<byte>();
		return (ttxData[..(int) tthLen], ttxData[^(int) ttzLen..]);
	}

	public bool Bake(string tthPath, string ttzPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ttx_bake_to_file(Handle, tthPath, ttzPath));
	}
}
