// ReSharper disable InconsistentNaming

using System;

namespace sourcepp.vtfpp;

public sealed class PSFrames : sourcepp.ManagedNativeHandle
{
	internal PSFrames(nint handle) : base(handle, DLL.vtfpp_psframes_close)
	{
	}

	public PSFrames(ReadOnlySpan<byte> buffer) : this(DLL.vtfpp_psframes_open_from_mem(buffer, (ulong) buffer.Length))
	{
	}

	public PSFrames(string psframesPath) : this(DLL.vtfpp_psframes_open_from_file(psframesPath))
	{
	}

	public static implicit operator bool(PSFrames frames)
	{
		frames.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_psframes_is_valid(frames.Handle));
	}

	public uint FrameCount
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_psframes_get_frame_count(Handle);
		}
	}

	public uint FPS
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_psframes_get_fps(Handle);
		}
	}

	public ushort GetWidth(uint frame)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_psframes_get_width(Handle, frame);
	}

	public ushort GetHeight(uint frame)
	{
		ThrowIfDisposed();
		return DLL.vtfpp_psframes_get_height(Handle, frame);
	}

	public byte[] GetPaletteDataRaw(uint frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_psframes_get_palette_data_raw(Handle, frame)).Read<byte>();
	}

	public byte[] GetPaletteDataAs(ImageFormat newFormat, uint frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_psframes_get_palette_data_as(Handle, newFormat, frame)).Read<byte>();
	}

	public byte[] GetImageDataRaw(uint frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_psframes_get_image_data_raw(Handle, frame)).Read<byte>();
	}

	public byte[] GetImageDataAs(ImageFormat newFormat, uint frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_psframes_get_image_data_as(Handle, newFormat, frame)).Read<byte>();
	}

	public byte[] GetImageDataAsBGR888(uint frame)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_psframes_get_image_data_as_bgr888(Handle, frame)).Read<byte>();
	}
}
