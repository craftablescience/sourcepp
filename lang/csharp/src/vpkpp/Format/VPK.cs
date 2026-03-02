using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class VPK : PackFile
{
	protected VPK(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public static VPK? Create(string path)
	{
		var handle = DLL.vpkpp_vpk_create(path);
		return handle == nint.Zero ? null : new VPK(handle);
	}

	public static VPK? Create(string path, uint version)
	{
		var handle = DLL.vpkpp_vpk_create_with_options(path, version);
		return handle == nint.Zero ? null : new VPK(handle);
	}

	public new static VPK? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_vpk_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new VPK(handle);
	}

	public static bool GenerateKeypairFiles(string path)
	{
		return Convert.ToBoolean(DLL.vpkpp_vpk_generate_keypair_files(path));
	}

	public bool Sign(string filename)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_vpk_sign_from_file(Handle, filename));
	}

	public bool Sign(ReadOnlySpan<byte> privateKeyBuffer, ReadOnlySpan<byte> publicKeyBuffer)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_vpk_sign_from_mem(Handle, privateKeyBuffer, (ulong) privateKeyBuffer.Length, publicKeyBuffer, (ulong) publicKeyBuffer.Length));
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_vpk_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_vpk_set_version(Handle, value);
		}
	}
	
	public uint ChunkSize
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_vpk_get_chunk_size(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vpkpp_vpk_set_chunk_size(Handle, value);
		}
	}
}
