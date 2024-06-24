using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern String sourcepp_string_new(ulong size);

        [DllImport("vpkppc")]
        public static extern void sourcepp_string_free(String* str);

        [DllImport("vpkppc")]
        public static extern StringArray sourcepp_string_array_new(ulong size);

        [DllImport("vpkppc")]
        public static extern void sourcepp_string_array_free(StringArray* array);
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct String
    {
        public long size;
        public unsafe sbyte* data;
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct StringArray
    {
        public long size;
        public unsafe sbyte** data;
    }

    internal static unsafe class StringUtils
    {
        public static string ConvertToStringAndDelete(ref String str)
        {
            var result = new string(str.data);

            fixed (String* strPtr = &str)
            {
                Extern.sourcepp_string_free(strPtr);
            }

            return result;
        }

        public static List<string> ConvertToListAndDelete(ref StringArray array)
        {
            var strings = new List<string>();

            for (long i = 0; i < array.size; i++)
            {
                strings.Add(new string(array.data[i]));
            }

            fixed (StringArray* arrayPtr = &array)
            {
                Extern.sourcepp_string_array_free(arrayPtr);
            }

            return strings;
        }
    }
}
