using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class VPK
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_create_with_options")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path, uint version);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_guid")]
			public static partial sourcepp.String GUID();

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_generate_keypair_files")]
			public static partial byte GenerateKeypairFiles([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_sign_from_file")]
			public static partial byte Sign(void* handle, [MarshalAs(UnmanagedType.LPStr)] string filepath);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_sign_from_mem")]
			public static partial byte Sign(void* handle, byte* privateKeyBuffer, ulong privateKeyLen, byte* publicKeyBuffer, ulong publicKeyLen);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_get_version")]
			public static partial uint GetVersion(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_set_version")]
			public static partial void SetVersion(void* handle, uint version);
		}
    }

    public class VPK : PackFile
    {
        private unsafe VPK(void* handle) : base(handle) {}

        public static VPK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.VPK.Create(path);
                return handle == null ? null : new VPK(handle);
            }
        }

        public static VPK? Create(string path, uint version)
        {
            unsafe
            {
                var handle = Extern.VPK.Create(path, version);
                return handle == null ? null : new VPK(handle);
            }
        }

        public new static VPK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.VPK.Open(path, 0);
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
                var handle = Extern.VPK.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new VPK(handle);
            }
        }

        public static bool GenerateKeyPairFiles(string path)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.VPK.GenerateKeypairFiles(path));
            }
        }

        public bool Sign(string filepath)
        {
            unsafe
            {
                return Convert.ToBoolean(Extern.VPK.Sign(Handle, filepath));
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
                        return Convert.ToBoolean(Extern.VPK.Sign(Handle, privateKeyBufferPtr, (ulong) privateKey.LongLength, publicKeyBufferPtr, (ulong) publicKey.LongLength));
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
                        return Convert.ToBoolean(Extern.VPK.Sign(Handle, privateKeyBufferPtr, (ulong) privateKeyData.LongLength, publicKeyBufferPtr, (ulong) publicKeyData.LongLength));
                    }
                }
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.VPK.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}

		public uint Version
        {
            get
            {
                unsafe
                {
                    return Extern.VPK.GetVersion(Handle);
                }
            }
            set
            {
                unsafe
                {
                    Extern.VPK.SetVersion(Handle, Math.Clamp(value, 1, 2));
                }
            }
        }
    }
}
