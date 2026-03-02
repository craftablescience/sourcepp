using System;
using System.Collections.Generic;

namespace sourcepp.vpkpp;

using OpenPropertyRequest = Func<PackFile, OpenProperty, byte[]>;

using EntryCallback = Action<string, Entry>;

using EntryPredicate = Func<string, Entry, bool>;

using EntryCreation = Func<string, EntryOptions>;

public enum OpenProperty
{
	DECRYPTION_KEY = 0,
}

public class PackFile : sourcepp.ManagedNativeHandle
{
	// ReSharper disable once MemberCanBeProtected.Global
	protected internal PackFile(nint handle, bool managed = true) : base(handle, DLL.vpkpp_pack_file_close, managed)
	{
	}

	public static PackFile? Open(string filename, EntryCallback? callback = null, OpenPropertyRequest? requestProperty = null)
	{
		var handle = DLL.vpkpp_pack_file_open(filename, callback is not null ? (path, entry) =>
		{
			callback(path, new Entry(entry, false));
		} : null, requestProperty is not null ? (handle, property) =>
		{
			var data = requestProperty(new PackFile(handle, false), property);
			return sourcepp.DLL.sourcepp_buffer_new((ulong) data.Length);
		} : null);
		return handle == nint.Zero ? null : new PackFile(handle);
	}

	public static List<string> OpenableExtensions => new sourcepp.StringArray(DLL.vpkpp_pack_file_get_openable_extensions()).Read();

	public bool HasEntryChecksums
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_pack_file_has_entry_checksums(Handle));
		}
	}

	public List<string> VerifyEntryChecksums()
	{
		ThrowIfDisposed();
		return new sourcepp.StringArray(DLL.vpkpp_pack_file_verify_entry_checksums(Handle)).Read();
	}

	public bool HasPackFileChecksum
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_pack_file_has_pack_file_checksum(Handle));
		}
	}
	
	public bool VerifyPackFileChecksum()
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_verify_pack_file_checksum(Handle));
	}
	
	public bool HasPackFileSignature
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_pack_file_has_pack_file_signature(Handle));
		}
	}
	
	public bool VerifyPackFileSignature()
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_verify_pack_file_signature(Handle));
	}

	public bool IsCaseSensitive
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_pack_file_is_case_sensitive(Handle));
		}
	}

	public bool HasEntry(string path, bool includeUnbaked = true)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_has_entry(Handle, path, Convert.ToInt32(includeUnbaked)));
	}

	public Entry? FindEntry(string path, bool includeUnbaked = true)
	{
		ThrowIfDisposed();
		var entry = DLL.vpkpp_pack_file_find_entry(Handle, path, Convert.ToInt32(includeUnbaked));
		return entry == nint.Zero ? null : new Entry(entry);
	}

	public byte[]? ReadEntry(string path)
	{
		ThrowIfDisposed();
		var data = new sourcepp.Buffer(DLL.vpkpp_pack_file_read_entry(Handle, path));
		return data.IsValid ? data.Read() : null;
	}
	
	public string? ReadEntryText(string path)
	{
		ThrowIfDisposed();
		var data = new sourcepp.String(DLL.vpkpp_pack_file_read_entry_text(Handle, path));
		return data.IsValid ? data.Read() : null;
	}

	public bool IsReadOnly
	{
		get
		{
			ThrowIfDisposed();
			return Convert.ToBoolean(DLL.vpkpp_pack_file_is_read_only(Handle));
		}
	}

	public bool AddEntry(string entryPath, string filePath, EntryOptions? options = null)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(options is null ? DLL.vpkpp_pack_file_add_entry_from_file(Handle, entryPath, filePath) : DLL.vpkpp_pack_file_add_entry_from_file_with_options(Handle, entryPath, filePath, options.GetValueOrDefault()));
	}

	public bool AddEntry(string entryPath, ReadOnlySpan<byte> buffer, EntryOptions? options = null)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(options is null ? DLL.vpkpp_pack_file_add_entry_from_mem(Handle, entryPath, buffer, (ulong) buffer.Length) : DLL.vpkpp_pack_file_add_entry_from_mem_with_options(Handle, entryPath, buffer, (ulong) buffer.Length, options.GetValueOrDefault()));
	}

	public long AddDirectory(string entryBaseDir, string dir, EntryOptions options)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_pack_file_add_directory_with_options(Handle, entryBaseDir, dir, options);
	}

	public long AddDirectory(string entryBaseDir, string dir, EntryCreation? creation = null)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_pack_file_add_directory(Handle, entryBaseDir, dir, creation is not null ? path => creation(path) : null);
	}

	public bool RenameEntry(string oldPath, string newPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_rename_entry(Handle, oldPath, newPath));
	}
	
	public bool RenameDirectory(string oldPath, string newPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_rename_directory(Handle, oldPath, newPath));
	}
	
	public bool RemoveEntry(string path)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_remove_entry(Handle, path));
	}
	
	public long RemoveDirectory(string dirPath)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_pack_file_remove_directory(Handle, dirPath);
	}

	public bool Bake(string outputDir = "", BakeOptions? options = null, EntryCallback? callback = null)
	{
		ThrowIfDisposed();
		DLL.EntryCallbackNative? callbackNative = callback is not null ? (path, entry) => callback(path, new Entry(entry, false)) : null;
		return Convert.ToBoolean(options is null ? DLL.vpkpp_pack_file_bake(Handle, outputDir, callbackNative) : DLL.vpkpp_pack_file_bake_with_options(Handle, outputDir, options.GetValueOrDefault(), callbackNative));
	}

	public bool ExtractEntry(string entryPath, string filePath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_extract_entry(Handle, entryPath, filePath));
	}
	
	public bool ExtractDirectory(string dir, string outputDir)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_extract_directory(Handle, dir, outputDir));
	}

	public bool ExtractAll(string outputDir, bool createUnderPackFileDir = true)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_extract_all(Handle, outputDir, Convert.ToInt32(createUnderPackFileDir)));
	}

	public bool ExtractAll(string outputDir, EntryPredicate predicate, bool stripSharedDirs = true)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vpkpp_pack_file_extract_all_if(Handle, outputDir, (path, entry) => Convert.ToInt32(predicate(path, new Entry(entry, false))), Convert.ToInt32(stripSharedDirs)));
	}

	public ulong EntryCount(bool includeUnbaked = true)
	{
		ThrowIfDisposed();
		return DLL.vpkpp_pack_file_get_entry_count(Handle, Convert.ToInt32(includeUnbaked));
	}

	public void RunForAllEntries(EntryCallback callback, bool includeUnbaked = true)
	{
		ThrowIfDisposed();
		DLL.vpkpp_pack_file_run_for_all_entries(Handle, (path, entry) => callback(path, new Entry(entry, false)), Convert.ToInt32(includeUnbaked));
	}
	
	public void RunForAllEntries(string parentDir, EntryCallback callback, bool recursive = true, bool includeUnbaked = true)
	{
		ThrowIfDisposed();
		DLL.vpkpp_pack_file_run_for_all_entries_under(Handle, parentDir, (path, entry) => callback(path, new Entry(entry, false)), Convert.ToInt32(recursive), Convert.ToInt32(includeUnbaked));
	}

	public string Filepath
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_filepath(Handle)).Read();
		}
	}
	
	public string TruncatedFilepath
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_truncated_filepath(Handle)).Read();
		}
	}
	
	public string Filename
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_filename(Handle)).Read();
		}
	}
	
	public string TruncatedFilename
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_truncated_filename(Handle)).Read();
		}
	}
	
	public string Filestem
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_filestem(Handle)).Read();
		}
	}
	
	public string TruncatedFilestem
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.vpkpp_pack_file_get_truncated_filestem(Handle)).Read();
		}
	}

	public Attribute SupportedEntryAttributes
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vpkpp_pack_file_get_supported_entry_attributes(Handle);
		}
	}

	public override string ToString()
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.vpkpp_pack_file_to_string(Handle)).Read();
	}

	public static string EscapeEntryPathForWrite(string path)
	{
		return new sourcepp.String(DLL.vpkpp_pack_file_escape_entry_path_for_write(path)).Read();
	}
}
