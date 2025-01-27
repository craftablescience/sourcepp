using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vpkpp
{
    using EntryCallback = Action<string, Entry>;

    using EntryPredicate = Func<string, Entry, bool>;

    using EntryCreation = Func<string, EntryOptions>;

    internal unsafe delegate void EntryCallbackNative(string path, void* entry);

    internal unsafe delegate int EntryPredicateNative(string path, void* entry);

    internal unsafe delegate EntryOptions EntryCreationNative(string path);

    internal static unsafe partial class Extern
    {
        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_open")]
        public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_openable_extensions")]
        public static partial sourcepp.StringArray GetOpenableExtensions();

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_guid")]
		public static partial sourcepp.String GetGUID(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_has_entry_checksums")]
        public static partial int HasEntryChecksums(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_verify_entry_checksums")]
        public static partial sourcepp.StringArray VerifyEntryChecksums(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_has_pack_file_checksum")]
        public static partial int HasPackFileChecksum(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_verify_pack_file_checksum")]
        public static partial int VerifyPackFileChecksum(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_has_pack_file_signature")]
        public static partial int HasPackFileSignature(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_verify_pack_file_signature")]
        public static partial int VerifyPackFileSignature(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_is_case_sensitive")]
        public static partial int IsCaseSensitive(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_has_entry")]
        public static partial int HasEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, int includeUnbaked);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_find_entry")]
        public static partial void* FindEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, int includeUnbaked);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_read_entry")]
        public static partial sourcepp.Buffer ReadEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_read_entry_text")]
        public static partial sourcepp.String ReadEntryText(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_is_read_only")]
        public static partial int IsReadOnly(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_entry_from_file")]
        public static partial void AddEntryFromFile(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_entry_from_file_with_options")]
        public static partial void AddEntryFromFile(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath, EntryOptions options);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_entry_from_mem")]
        public static partial void AddEntryFromMem(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, byte* buffer, ulong bufferLen);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_entry_from_mem_with_options")]
        public static partial void AddEntryFromMem(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, byte* buffer, ulong bufferLen, EntryOptions options);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_directory")]
        public static partial void AddDirectory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryBaseDir, [MarshalAs(UnmanagedType.LPStr)] string dir, IntPtr creation);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_add_directory_with_options")]
        public static partial void AddDirectory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryBaseDir, [MarshalAs(UnmanagedType.LPStr)] string dir, EntryOptions options);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_rename_entry")]
        public static partial int RenameEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string oldPath, [MarshalAs(UnmanagedType.LPStr)] string newPath);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_rename_directory")]
        public static partial int RenameDirectory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string oldDir, [MarshalAs(UnmanagedType.LPStr)] string newDir);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_remove_entry")]
        public static partial int RemoveEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_remove_directory")]
        public static partial ulong RemoveDirectory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string dirName);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_bake")]
        public static partial int Bake(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, IntPtr callback);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_bake_with_options")]
        public static partial int Bake(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, BakeOptions options, IntPtr callback);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_extract_entry")]
        public static partial int ExtractEntry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string entryPath, [MarshalAs(UnmanagedType.LPStr)] string filepath);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_extract_directory")]
        public static partial int ExtractDirectory(void* handle, [MarshalAs(UnmanagedType.LPStr)] string dir, [MarshalAs(UnmanagedType.LPStr)] string outputDir);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_extract_all")]
        public static partial int ExtractAll(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, int createUnderPackFileDir);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_extract_all_if")]
        public static partial int ExtractAllIf(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, IntPtr predicate, int stripSharedDirs);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_entry_count")]
        public static partial ulong GetEntryCount(void* handle, int includeUnbaked);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_run_for_all_entries")]
        public static partial void RunForAllEntries(void* handle, IntPtr operation, int includeUnbaked);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_filepath")]
        public static partial sourcepp.String GetFilepath(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_truncated_filepath")]
        public static partial sourcepp.String GetTruncatedFilepath(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_filename")]
        public static partial sourcepp.String GetFilename(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_truncated_filename")]
        public static partial sourcepp.String GetTruncatedFilename(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_filestem")]
        public static partial sourcepp.String GetFilestem(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_truncated_filestem")]
        public static partial sourcepp.String GetTruncatedFilestem(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_get_supported_entry_attributes")]
        public static partial Attribute GetSupportedEntryAttributes(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_to_string")]
        public static partial sourcepp.String ToString(void* handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_close")]
        public static partial void Close(void** handle);

        [LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_escape_entry_path_for_write")]
        public static partial sourcepp.String EscapeEntryPathForWrite([MarshalAs(UnmanagedType.LPStr)] string path);
    }

    public class PackFile
    {
        private protected unsafe PackFile(void* handle)
        {
            Handle = handle;
        }

        ~PackFile()
        {
            unsafe
            {
                fixed (void** handlePtr = &Handle)
                {
                    Extern.Close(handlePtr);
                }
            }
        }

        public static PackFile? Open(string path)
        {
            unsafe
            {
                var handle = Extern.Open(path, 0);
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static PackFile? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static string EscapeEntryPathForWrite(string path)
        {
            var str = Extern.EscapeEntryPathForWrite(path);
            return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
        }

        public static List<string> GetSupportedFileTypes()
        {
            var stringArray = Extern.GetOpenableExtensions();
            return sourcepp.StringUtils.ConvertToListAndDelete(ref stringArray);
        }

        public bool HasEntryChecksums()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.HasEntryChecksums(Handle));
            }
        }

        public IEnumerable<string> VerifyEntryChecksums()
        {
            unsafe
            {
                var stringArray = Extern.VerifyEntryChecksums(Handle);
                return sourcepp.StringUtils.ConvertToListAndDelete(ref stringArray);
            }
        }

        public bool HasPackFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.HasPackFileChecksum(Handle));
            }
        }

        public bool VerifyPackFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.VerifyPackFileChecksum(Handle));
            }
        }

        public bool HasPackFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.HasPackFileSignature(Handle));
            }
        }

        public bool VerifyPackFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.VerifyPackFileSignature(Handle));
            }
        }

        public bool HasEntry(string path, bool includeUnbaked = true)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.HasEntry(Handle, path, Convert.ToInt32(includeUnbaked)));
            }
        }

        public Entry? FindEntry(string path, bool includeUnbaked = true)
        {
            unsafe
            {
                var entry = Extern.FindEntry(Handle, path, Convert.ToInt32(includeUnbaked));
                return entry == null ? null : new Entry(entry, false);
            }
        }

        public byte[]? ReadEntry(string path)
        {
            unsafe
            {
                var buffer = Extern.ReadEntry(Handle, path);
                return buffer.size < 0 ? null : sourcepp.BufferUtils.ConvertToArrayAndDelete(ref buffer);
            }
        }

        public string? ReadEntryText(string path)
        {
            unsafe
            {
                var str = Extern.ReadEntryText(Handle, path);
                return str.size < 0 ? null : sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
            }
        }

        public void AddEntry(string entryPath, string filepath)
        {
            unsafe
            {
                Extern.AddEntryFromFile(Handle, entryPath, filepath);
            }
        }

        public void AddEntry(string entryPath, string filepath, EntryOptions options)
        {
            unsafe
            {
                Extern.AddEntryFromFile(Handle, entryPath, filepath, options);
            }
        }

        public void AddEntry(string path, byte[] buffer)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Extern.AddEntryFromMem(Handle, path, bufferPtr, (ulong) buffer.LongLength);
                }
            }
        }

        public void AddEntry(string path, byte[] buffer, EntryOptions options)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Extern.AddEntryFromMem(Handle, path, bufferPtr, (ulong) buffer.LongLength, options);
                }
            }
        }

        public void AddEntry(string path, IEnumerable<byte> buffer)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Extern.AddEntryFromMem(Handle, path, bufferPtr, (ulong)data.LongLength);
                }
            }
        }

        public void AddEntry(string path, IEnumerable<byte> buffer, EntryOptions options)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Extern.AddEntryFromMem(Handle, path, bufferPtr, (ulong)data.LongLength, options);
                }
            }
        }

        public void AddDirectory(string entryBasePath, string dir)
        {
            unsafe
            {
                Extern.AddDirectory(Handle, entryBasePath, dir, 0);
            }
        }

        public void AddDirectory(string entryBasePath, string dir, EntryOptions options)
        {
            unsafe
            {
                Extern.AddDirectory(Handle, entryBasePath, dir, options);
            }
        }

        public void AddDirectory(string entryBasePath, string dir, EntryCreation creation)
        {
            unsafe
            {
                EntryCreationNative callbackNative = (path) =>
                {
                    return creation(path);
                };
                Extern.AddDirectory(Handle, entryBasePath, dir, Marshal.GetFunctionPointerForDelegate(callbackNative));
            }
        }

        public bool RenameEntry(string oldPath, string newPath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.RenameEntry(Handle, oldPath, newPath));
            }
        }

        public bool RenameDirectory(string oldDir, string newDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.RenameDirectory(Handle, oldDir, newDir));
            }
        }

        public bool RemoveEntry(string path)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.RemoveEntry(Handle, path));
            }
        }

        public ulong RemoveDirectory(string dirName)
        {
            unsafe
            {
                return Extern.RemoveDirectory(Handle, dirName);
            }
        }

        public bool Bake(string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.Bake(Handle, outputDir, 0));
            }
        }

        public bool Bake(string outputDir, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                return Convert.ToBoolean(Extern.Bake(Handle, outputDir, Marshal.GetFunctionPointerForDelegate(callbackNative)));
            }
        }

        public bool Bake(string outputDir, BakeOptions options)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.Bake(Handle, outputDir, options, 0));
            }
        }

        public bool Bake(string outputDir, BakeOptions options, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                return Convert.ToBoolean(Extern.Bake(Handle, outputDir, options, Marshal.GetFunctionPointerForDelegate(callbackNative)));
            }
        }

        public bool ExtractEntry(string entryPath, string filepath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.ExtractEntry(Handle, entryPath, filepath));
            }
        }

        public bool ExtractDirectory(string dir, string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.ExtractDirectory(Handle, dir, outputDir));
            }
        }

        public bool ExtractAll(string outputDir, bool createUnderPackFileDir = true)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.ExtractAll(Handle, outputDir, Convert.ToInt32(createUnderPackFileDir)));
            }
        }

        public bool ExtractAll(string outputDir, EntryPredicate predicate, bool stripSharedDirs = true)
        {
            unsafe
            {
                EntryPredicateNative predicateNative = (path, entry) =>
                {
                    return Convert.ToInt32(predicate(path, new Entry(entry, true)));
                };
                return Convert.ToBoolean(Extern.ExtractAllIf(Handle, outputDir, Marshal.GetFunctionPointerForDelegate(predicateNative), Convert.ToInt32(stripSharedDirs)));
            }
        }

        public ulong GetEntryCount(bool includeUnbaked = true)
        {
            unsafe
            {
                return Extern.GetEntryCount(Handle, Convert.ToInt32(includeUnbaked));
            }
        }

        public void RunForAllEntries(EntryCallback operation, bool includeUnbaked = true)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    operation(path, new Entry(entry, true));
                };
                Extern.RunForAllEntries(Handle, Marshal.GetFunctionPointerForDelegate(callbackNative), Convert.ToInt32(includeUnbaked));
            }
        }

        public override string ToString()
        {
            unsafe
            {
                var str = Extern.ToString(Handle);
                return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
            }
        }

        public string TypeGUID
        {
            get
            {
                unsafe
                {
					var str = Extern.GetGUID(Handle);
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public bool CaseSensitive
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.IsCaseSensitive(Handle));
                }
            }
        }

        public bool ReadOnly
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.IsReadOnly(Handle));
                }
            }
        }

        public string FilePath
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetFilepath(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFilePath
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetTruncatedFilepath(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string FileName
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetFilename(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFileName
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetTruncatedFilename(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string FileStem
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetFilestem(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public string TruncatedFileStem
        {
            get
            {
                unsafe
                {
                    var str = Extern.GetTruncatedFilestem(Handle);
                    return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
                }
            }
        }

        public Attribute SupportedEntryAttributes
        {
            get
            {
                unsafe
                {
                    return Extern.GetSupportedEntryAttributes(Handle);
                }
            }
        }

        private protected readonly unsafe void* Handle;
    }
}
