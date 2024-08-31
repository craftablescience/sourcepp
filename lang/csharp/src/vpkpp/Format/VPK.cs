using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_create([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_create_with_options([MarshalAs(UnmanagedType.LPStr)] string path, uint version);

        [DllImport("vpkppc")]
        public static extern void* vpkpp_vpk_open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_vpk_generate_keypair_files([MarshalAs(UnmanagedType.LPStr)] string path);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_vpk_sign_from_file(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filepath);

        [DllImport("vpkppc")]
        public static extern byte vpkpp_vpk_sign_from_mem(void* handle, byte* privateKeyBuffer, ulong privateKeyLen, byte* publicKeyBuffer, ulong publicKeyLen);

        [DllImport("vpkppc")]
        public static extern uint vpkpp_vpk_get_version(void* handle);

        [DllImport("vpkppc")]
        public static extern void vpkpp_vpk_set_version(void* handle, uint version);
    }

    public class VPK : PackFile
    {
        private unsafe VPK(void* handle) : base(handle) {}

        public static VPK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_create(path);
                return handle == null ? null : new VPK(handle);
            }
        }

        public static VPK? Create(string path, uint version)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_create_with_options(path, version);
                return handle == null ? null : new VPK(handle);
            }
        }

        public new static VPK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.vpkpp_vpk_open(path, 0);
                return handle == null ? null : new VPK(handle);
            }
        }

        public new static VPK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.vpkpp_vpk_open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new VPK(handle);
            }
        }

        public static bool GenerateKeyPairFiles(string path)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_vpk_generate_keypair_files(path));
            }
        }

        public bool Sign(string filepath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.vpkpp_vpk_sign_from_file(Handle, filepath));
            }
        }

        public bool Sign(byte[] privateKey, byte[] publicKey)
        {
            unsafe
            {
                fixed (byte* privateKeyBufferPtr = privateKey)
                {
                    fixed (byte* publicKeyBufferPtr = publicKey)
                    {
                        return Convert.ToBoolean(Extern.vpkpp_vpk_sign_from_mem(Handle, privateKeyBufferPtr, (ulong) privateKey.LongLength, publicKeyBufferPtr, (ulong) publicKey.LongLength));
                    }
                }
            }
        }

        public bool Sign(IEnumerable<byte> privateKey, IEnumerable<byte> publicKey)
        {
            var privateKeyData = privateKey.ToArray();
            var publicKeyData = publicKey.ToArray();
            unsafe
            {
                fixed (byte* privateKeyBufferPtr = privateKeyData)
                {
                    fixed (byte* publicKeyBufferPtr = publicKeyData)
                    {
                        return Convert.ToBoolean(Extern.vpkpp_vpk_sign_from_mem(Handle, privateKeyBufferPtr, (ulong) privateKeyData.LongLength, publicKeyBufferPtr, (ulong) publicKeyData.LongLength));
                    }
                }
            }
        }

        public uint Version
        {
            get
            {
                unsafe
                {
                    return Extern.vpkpp_vpk_get_version(Handle);
                }
            }
            set
            {
                unsafe
                {
                    Extern.vpkpp_vpk_set_version(Handle, Math.Clamp(value, 1, 2));
                }
            }
        }
    }
}
