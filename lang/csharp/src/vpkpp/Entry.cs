using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_path(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_parent_path(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_filename(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_stem(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern ulong vpkpp_entry_get_extension(void* handle, sbyte* buffer, ulong bufferLen);

        [DllImport("vpkppc")]
        public static extern void vpkpp_entry_free(void** handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_entry_array_free(EntryHandleArray* array);
    }

    [StructLayout(LayoutKind.Sequential)]
    internal unsafe struct EntryHandleArray
    {
        internal long size;
        internal void** data;
    }

    public class Entry
    {
        internal unsafe Entry(void* handle, bool inArray)
        {
            Handle = handle;
            _inArray = inArray;
        }

        ~Entry()
        {
            if (!_inArray)
            {
                unsafe
                {
                    fixed (void** handlePtr = &Handle)
                    {
                        Extern.vpkpp_entry_free(handlePtr);
                    }
                }
            }
        }

        public string Path
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxPath];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_entry_get_path(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string ParentPath
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxPath];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_entry_get_parent_path(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
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
                        Extern.vpkpp_entry_get_filename(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string Stem
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_entry_get_stem(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        public string Extension
        {
            get
            {
                Span<sbyte> stringArray = new sbyte[Constants.MaxFilename];
                unsafe
                {
                    fixed (sbyte* stringPtr = stringArray)
                    {
                        Extern.vpkpp_entry_get_extension(Handle, stringPtr, Convert.ToUInt64(stringArray.Length));
                        return new string(stringPtr);
                    }
                }
            }
        }

        internal unsafe void* Handle;

        private readonly bool _inArray;
    }

    public class EntryEnumerable : IEnumerable<Entry>
    {
        internal EntryEnumerable(EntryHandleArray array)
        {
            _array = array;
        }

        ~EntryEnumerable()
        {
            unsafe
            {
                fixed (EntryHandleArray* arrayPtr = &_array)
                {
                    Extern.vpkpp_entry_array_free(arrayPtr);
                }
            }
        }

        private Entry GetEntryAtPosition(ulong pos)
        {
            unsafe
            {
                return new Entry(_array.data[pos], true);
            }
        }

        private IEnumerator<Entry> GetEnumerator()
        {
            for (long i = 0; i < _array.size; i++)
            {
                yield return GetEntryAtPosition((ulong) i);
            }
        }

        IEnumerator<Entry> IEnumerable<Entry>.GetEnumerator()
        {
            return GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        private EntryHandleArray _array;
    }
}
