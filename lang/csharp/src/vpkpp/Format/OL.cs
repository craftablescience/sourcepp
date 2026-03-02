using System;

namespace sourcepp.vpkpp.Format;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

public class OL : PackFile
{
	protected OL(nint handle, bool managed = true) : base(handle, managed)
	{
	}

	public new static OL? Open(string path, EntryCallback? callback = null, OpenPropertyRequest? _ = null)
	{
		var handle = DLL.vpkpp_ol_open(path, callback is not null ? (entryPath, entry) =>
		{
			callback(entryPath, new Entry(entry, false));
		} : null);
		return handle == nint.Zero ? null : new OL(handle);
	}

	public string Notes
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_ol_get_notes(Handle)).Read();
		}
	}

	public string EntryNotes(string path)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.vpkpp_ol_get_entry_notes(Handle, path)).Read();
	}
}
