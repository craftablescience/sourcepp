﻿using System;
using System.Runtime.InteropServices;

namespace vpkpp
{
    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern Buffer sourcepp_buffer_new(ulong size);

        [DllImport("vpkppc")]
        public static extern void sourcepp_buffer_free(Buffer* str);
    }

    [StructLayout(LayoutKind.Sequential)]
    internal struct Buffer
    {
        public long size;
        public unsafe byte* data;
    }

    internal static unsafe class BufferUtils
    {
        public static byte[] ConvertToArrayAndDelete(ref Buffer buffer)
        {
            var result = new Span<byte>(buffer.data, (int) buffer.size).ToArray();

            fixed (Buffer* bufferPtr = &buffer)
            {
                Extern.sourcepp_buffer_free(bufferPtr);
            }

            return result;
        }
    }
}
