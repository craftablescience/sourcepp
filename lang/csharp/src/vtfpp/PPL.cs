using System;

namespace sourcepp.vtfpp;

public sealed class PPL : sourcepp.ManagedNativeHandle
{
	internal PPL(nint handle) : base(handle, DLL.vtfpp_ppl_close)
	{
	}

	public PPL(uint modelChecksum, ImageFormat format = ImageFormat.RGB888, uint version = 0) : this(DLL.vtfpp_ppl_create_with_options(modelChecksum, format, version))
	{
	}

	public PPL(ReadOnlySpan<byte> buffer) : this(DLL.vtfpp_ppl_open_from_mem(buffer, (ulong) buffer.Length))
	{
	}

	public PPL(string pplPath) : this(DLL.vtfpp_ppl_open_from_file(pplPath))
	{
	}

	public static implicit operator bool(PPL ppl)
	{
		ppl.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_is_valid(ppl.Handle));
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ppl_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ppl_set_version(Handle, value);
		}
	}

	public uint ModelChecksum
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_ppl_get_model_checksum(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_ppl_set_model_checksum(Handle, value);
		}
	}

	public ImageFormat Format()
	{
		ThrowIfDisposed();
		return DLL.vtfpp_ppl_get_format(Handle);
	}

	public void Format(ImageFormat format, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		DLL.vtfpp_ppl_set_format(Handle, format, quality);
	}

	public bool HasImageForLod(uint lod)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_has_image_for_lod(Handle, lod));
	}

	public uint[] ImageLods
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.Buffer(DLL.vtfpp_ppl_get_image_lods(Handle)).Read<uint>();
		}
	}

	public Span<byte> GetImageRaw(ref uint width, ref uint height, uint lod = 0)
	{
		ThrowIfDisposed();
		unsafe
		{
			ulong imageLen = 0;
			var imagePtr = DLL.vtfpp_ppl_get_image_raw(Handle, ref imageLen, ref width, ref height, lod);
			return new Span<byte>(imagePtr, (int) imageLen);
		}
	}

	public byte[] GetImageAs(ref uint width, ref uint height, ImageFormat format, uint lod = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_ppl_get_image_as(Handle, ref width, ref height, format, lod)).Read<byte>();
	}
	
	// ReSharper disable once InconsistentNaming
	public byte[] GetImageAsRGB888(ref uint width, ref uint height, uint lod = 0)
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_ppl_get_image_as_rgb888(Handle, ref width, ref height, lod)).Read<byte>();
	}

	public bool SetImage(string imagePath, uint lod = 0, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_set_image_from_file(Handle, imagePath, lod, quality));
	}
	
	public bool SetImage(string imagePath, uint resizedWidth, uint resizedHeight, uint lod = 0, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_set_image_from_file_with_options(Handle, imagePath, resizedWidth, resizedHeight, lod, filter, quality));
	}

	public bool SetImage(ReadOnlySpan<byte> buffer, ImageFormat format, uint width, uint height, uint lod = 0, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_set_image_from_mem(Handle, buffer, (ulong) buffer.Length, format, width, height, lod, quality));
	}
	
	public bool SetImage(ReadOnlySpan<byte> buffer, ImageFormat format, uint width, uint height, uint resizedWidth, uint resizedHeight, uint lod = 0, ImageConversion.ResizeFilter filter = ImageConversion.ResizeFilter.DEFAULT, float quality = ImageConversion.DEFAULT_COMPRESSED_QUALITY)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_set_image_from_mem_with_options(Handle, buffer, (ulong) buffer.Length, format, width, height, resizedWidth, resizedHeight, lod, filter, quality));
	}

	public bool RemoveImage(uint lod)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_remove_image(Handle, lod));
	}

	public bool SaveImageToFile(string imagePath, uint lod, ImageConversion.FileFormat fileFormat = ImageConversion.FileFormat.DEFAULT)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_save_image_to_file(Handle, imagePath, lod, fileFormat));
	}

	public byte[] Bake()
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_ppl_bake(Handle)).Read<byte>();
	}

	public bool Bake(string pplPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_ppl_bake_to_file(Handle, pplPath));
	}
}
