using System;

namespace sourcepp.vpkpp.Format;

using EntryCallback = Action<string, Entry>;
using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

public class GCF : PackFile
{
	protected GCF(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public new static GCF? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? requestProperty = null)
	{
		var handle = DLL.vpkpp_gcf_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null, requestProperty is not null ? (handle, property) =>
		{
			var data = requestProperty(new PackFile(handle, false), property);
			return sourcepp.DLL.sourcepp_buffer_new((ulong) data.Length);
		} : null);
		return handle == nint.Zero ? null : new GCF(handle);
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_gcf_get_version(Handle);
		}
	}

	public uint AppId
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_gcf_get_appid(Handle);
		}
	}

	public uint AppVersion
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_gcf_get_app_version(Handle);
		}
	}
}
