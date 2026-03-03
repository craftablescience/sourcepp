using System;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

public sealed class HOT : sourcepp.ManagedNativeHandle
{
	[StructLayout(LayoutKind.Sequential, Pack = 1)]
	public struct Rect
	{
		[Flags]
		public enum Flags : byte
		{
			NONE              = 0,
			RANDOM_ROTATION   = 1 << 0,
			RANDOM_REFLECTION = 1 << 1,
			IS_ALTERNATE      = 1 << 2,
		}

		public Flags RectFlags;
		public ushort X1;
		public ushort Y1;
		public ushort X2;
		public ushort Y2;
	}

	internal HOT(nint handle) : base(handle, DLL.vtfpp_hot_close)
	{
	}

	public HOT() : this(DLL.vtfpp_hot_create())
	{
	}

	public HOT(ReadOnlySpan<byte> hotData) : this(DLL.vtfpp_hot_open_from_mem(hotData, (ulong) hotData.Length))
	{
	}
	
	public HOT(string hotPath) : this(DLL.vtfpp_hot_open_from_file(hotPath))
	{
	}
	
	public static implicit operator bool(HOT hot)
	{
		hot.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_hot_is_valid(hot.Handle));
	}

	public byte Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_hot_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_hot_set_version(Handle, value);
		}
	}

	public byte Flags
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_hot_get_flags(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_hot_set_flags(Handle, value);
		}
	}

	public Rect[] Rects
	{
		get
		{
			ThrowIfDisposed();
			var rects = new Rect[DLL.vtfpp_hot_get_rect_count(Handle)];
			for (var i = 0; i < rects.Length; i++)
			{
				rects[i] = DLL.vtfpp_hot_get_rect_at_index(Handle, (ushort) i);
			}
			return rects;
		}
		set
		{
			ThrowIfDisposed();
			var storedRectCount = DLL.vtfpp_hot_get_rect_count(Handle);
			for (var i = 0; i < value.Length; i++)
			{
				if (i < storedRectCount)
				{
					DLL.vtfpp_hot_set_rect_at_index(Handle, (ushort) i, value[i]);
				}
				else
				{
					DLL.vtfpp_hot_add_rect(Handle, value[i]);
				}
			}
			for (var i = storedRectCount - value.Length - 1; i >= 0; i--)
			{
				DLL.vtfpp_hot_remove_rect_at_index(Handle, (ushort) i);
			}
		}
	}

	public byte[] Bake()
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_hot_bake(Handle)).Read<byte>();
	}

	public bool Bake(string hotPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_hot_bake_to_file(Handle, hotPath));
	}
}
