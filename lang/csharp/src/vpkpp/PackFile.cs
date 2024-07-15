using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_open([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_open_with_options([MarshalAs(UnmanagedType.LPStr)] string path, PackFileOptions options);

        [DllImport("vpkppc")]
        public static extern PackFileType vpkpp_get_type(void* handle);

        [DllImport("vpkppc")]
        public static extern PackFileOptions vpkpp_get_options(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_has_entry_checksums(void* handle);

        [DllImport("vpkppc")]
        public static extern StringArray vpkpp_verify_entry_checksums(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_has_file_checksum(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_verify_file_checksum(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_has_file_signature(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_verify_file_signature(void* handle);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_is_case_sensitive(void* handle);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_find_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filename, byte includeUnbaked);

        [DllImport("vpkppc")]
        public static extern Buffer vpkpp_read_entry(void* handle, void* entry);

        [DllImport("vpkppc")]
        public static extern String vpkpp_read_entry_text(void* handle, void* entry);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_is_read_only(void* handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_file(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filename, [MarshalAs(UnmanagedType.LPStr)] string pathToFile);

        [DllImport("vpkppc")]
        public static extern void vpkpp_add_entry_from_mem(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filename, byte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_remove_entry(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filename);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_bake(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_extract_entry(void* handle, void* entry, [MarshalAs(UnmanagedType.LPStr)] string filePath);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_extract_dir(void* handle, [MarshalAs(UnmanagedType.LPStr)] string dir, [MarshalAs(UnmanagedType.LPStr)] string outputDir);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_extract_all(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, byte createUnderPackFileDir);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_extract_all_if(void* handle, [MarshalAs(UnmanagedType.LPStr)] string outputDir, IntPtr predicate);

        [DllImport("vpkppc")]
        public static extern EntryHandleArray vpkpp_get_baked_entries(void* handle);

        [DllImport("vpkppc")]
        public static extern EntryHandleArray vpkpp_get_unbaked_entries(void* handle);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_entry_count(void* handle, byte includeUnbaked);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_filepath(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_truncated_filepath(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_filename(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_truncated_filename(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_filestem(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_truncated_filestem(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_get_supported_entry_attributes(void* handle, Attribute* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_to_string(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern void vpkpp_close(void** handle);

        [DllImport("vpkppc")]
        public static extern String vpkpp_escape_entry_path([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern StringArray vpkpp_get_supported_file_types();
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
                    Extern.vpkpp_close(handlePtr);
                }
            }
        }

        public static PackFile? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_open(path);
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static PackFile? Open(string path, PackFileOptions options)
        {
            unsafe
            {
                var handle = Extern.vpkpp_open_with_options(path, options);
                return handle == null ? null : new PackFile(handle);
            }
        }

        public static string EscapeEntryPath(string path)
        {
            var str = Extern.vpkpp_escape_entry_path(path);
            return StringUtils.ConvertToStringAndDelete(ref str);
        }

        public static List<string> GetSupportedFileTypes()
        {
            var stringArray = Extern.vpkpp_get_supported_file_types();
            return StringUtils.ConvertToListAndDelete(ref stringArray);
        }

        public bool HasEntryChecksums()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_entry_checksums(Handle));
            }
        }

        public IEnumerable<string> VerifyEntryChecksums()
        {
            unsafe
            {
                var stringArray = Extern.vpkpp_verify_entry_checksums(Handle);
                return StringUtils.ConvertToListAndDelete(ref stringArray);
            }
        }

        public bool HasFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_file_checksum(Handle));
            }
        }

        public bool VerifyFileChecksum()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_verify_file_checksum(Handle));
            }
        }

        public bool HasFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_has_file_signature(Handle));
            }
        }

        public bool VerifyFileSignature()
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_verify_file_signature(Handle));
            }
        }

        public Entry? FindEntry(string filename, bool includeUnbaked = true)
        {
            unsafe
            {
                var entry = Extern.vpkpp_find_entry(Handle, filename, Convert.ToByte(includeUnbaked));
                return entry == null ? null : new Entry(entry, false);
            }
        }

        public byte[]? ReadEntry(Entry entry)
        {
            unsafe
            {
                var buffer = Extern.vpkpp_read_entry(Handle, entry.Handle);
                return buffer.size < 0 ? null : BufferUtils.ConvertToArrayAndDelete(ref buffer);
            }
        }

        public string? ReadEntryText(Entry entry)
        {
            unsafe
            {
                var str = Extern.vpkpp_read_entry_text(Handle, entry.Handle);
                return str.size < 0 ? null : StringUtils.ConvertToStringAndDelete(ref str);
            }
        }

        public void AddEntry(string filename, string pathToFile)
        {
            unsafe
            {
                Extern.vpkpp_add_entry_from_file(Handle, filename, pathToFile);
            }
        }

        public void AddEntry(string filename, byte[] buffer)
        {
            unsafe
            {
                fixed (byte* bufferPtr = buffer)
                {
                    Extern.vpkpp_add_entry_from_mem(Handle, filename, bufferPtr, (ulong) buffer.LongLength);
                }
            }
        }

        public void AddEntry(string filename, IEnumerable<byte> buffer)
        {
            unsafe
            {
                var data = buffer.ToArray();
                fixed (byte* bufferPtr = data)
                {
                    Extern.vpkpp_add_entry_from_mem(Handle, filename, bufferPtr, (ulong) data.LongLength);
                }
            }
        }

        public bool RemoveEntry(string filename)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_remove_entry(Handle, filename));
            }
        }

        public bool Bake(string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_bake(Handle, outputDir));
            }
        }

        public bool ExtractEntry(Entry entry, string filePath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_entry(Handle, entry.Handle, filePath));
            }
        }

        public bool ExtractDir(string dir, string outputDir)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_dir(Handle, dir, outputDir));
            }
        }

        public bool ExtractAll(string outputDir, bool createUnderPackFileDir = true)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_extract_all(Handle, outputDir, Convert.ToByte(createUnderPackFileDir)));
            }
        }

        private unsafe delegate byte ExtractAllPredicateNative(void* entry);

        public bool ExtractAll(string outputDir, Func<Entry, bool> predicate)
        {
            unsafe
            {
                ExtractAllPredicateNative predicateWrapper = (entry) =>
                {
                    // HACK: tell the entry its in an array so it doesn't kill itself
                    return Convert.ToByte(predicate(new Entry(entry, true)));
                };
                return Convert.ToBoolean(Extern.vpkpp_extract_all_if(Handle, outputDir, Marshal.GetFunctionPointerForDelegate(predicateWrapper)));
            }
        }

        public EntryEnumerable GetBakedEntries()
        {
            unsafe
            {
                return new EntryEnumerable(Extern.vpkpp_get_baked_entries(Handle));
            }
        }

        public EntryEnumerable GetUnbakedEntries()
        {
            unsafe
            {
                return new EntryEnumerable(Extern.vpkpp_get_unbaked_entries(Handle));
            }
        }

        public ulong GetEntryCount(bool includeUnbaked = true)
        {
            unsafe
            {
                return Extern.vpkpp_get_entry_count(Handle, Convert.ToByte(includeUnbaked));
            }
        }

        public override string ToString()
        {
            unsafe
            {
                var stringPtr = stackalloc sbyte[Constants.MaxPackFileString];
                Extern.vpkpp_to_string(Handle, stringPtr, Convert.ToUInt64(Constants.MaxPackFileString));
                return new string(stringPtr);
            }
        }

        public PackFileType Type
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_get_type(Handle);
                }
            }
        }

        public PackFileOptions Options
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_get_options(Handle);
                }
            }
        }

        public bool CaseSensitive
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.vpkpp_is_case_sensitive(Handle));
                }
            }
        }

        public bool ReadOnly
        {
            get
            {
                unsafe
                {
                    return Convert.ToBoolean(Extern.vpkpp_is_read_only(Handle));
                }
            }
        }

        public string FilePath
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxPath];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_filepath(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string TruncatedFilePath
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxPath];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_truncated_filepath(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string FileName
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_filename(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string TruncatedFileName
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_truncated_filename(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string FileStem
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_filestem(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string TruncatedFileStem
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_get_truncated_filestem(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public List<Attribute> SupportedEntryAttributes
        {
            get
            {
                unsafe
                {
                    var attrArray = stackalloc Attribute[(int) Attribute.COUNT];
                    var numAttributes = Extern.vpkpp_get_supported_entry_attributes(Handle, attrArray, (ulong) Attribute.COUNT);

                    var result = new List<Attribute>();
                    for (ulong i = 0; i < numAttributes; i++)
                    {
                        result.Add(attrArray[i]);
                    }
                    return result;
                }
            }
        }

        private protected readonly unsafe void* Handle;
    }
}
