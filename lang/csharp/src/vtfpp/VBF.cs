using System;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

public sealed class VBF : sourcepp.ManagedNativeHandle
{
	[Flags]
	public enum Flags : ushort {
		NONE        = 0,
		BOLD        = 1 << 0,
		ITALIC      = 1 << 1,
		OUTLINE     = 1 << 2,
		DROP_SHADOW = 1 << 3,
		BLUR        = 1 << 4,
		SCANLINE    = 1 << 5,
		ANTIALIASED = 1 << 6,
		CUSTOM      = 1 << 7,
	}
	
	[StructLayout(LayoutKind.Sequential)]
	public struct Glyph
	{
		public ushort x;
		public ushort y;
		public ushort w;
		public ushort h;
		public short a;
		public short b;
		public short c;
	}

	internal VBF(nint handle) : base(handle, DLL.vtfpp_vbf_close)
	{
	}

	public VBF(ReadOnlySpan<byte> buffer) : this(DLL.vtfpp_vbf_open_from_mem(buffer, (ulong) buffer.Length))
	{
	}

	public VBF(string vbfPath) : this(DLL.vtfpp_vbf_open_from_file(vbfPath))
	{
	}
	
	public static implicit operator bool(VBF vbf)
	{
		vbf.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_vbf_is_valid(vbf.Handle));
	}

	public (ushort, ushort) PageSize
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.vtfpp_vbf_get_page_width(Handle), DLL.vtfpp_vbf_get_page_height(Handle));
		}
	}

	public (ushort, ushort) MaxGlyphSize
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.vtfpp_vbf_get_max_glyph_width(Handle), DLL.vtfpp_vbf_get_max_glyph_height(Handle));
		}
	}

	// ReSharper disable once InconsistentNaming
	public Flags VBFFlags
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vbf_get_flags(Handle);
		}
	}

	public ushort Ascent
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_vbf_get_ascent(Handle);
		}
	}

	public Glyph[] Glyphs
	{
		get
		{
			ThrowIfDisposed();
			var glyphs = new Glyph[256];
			for (byte i = 0; i < glyphs.Length; i++)
			{
				glyphs[i] = DLL.vtfpp_vbf_get_glyph(Handle, i);
			}
			return glyphs;
		}
	}
}
