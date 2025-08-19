using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class PAK
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_create_with_options")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path, int hrot);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_guid")]
			public static partial sourcepp.String GUID();

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_get_type")]
			public static partial int GetType(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_pak_set_type")]
			public static partial void SetType(void* handle, int type);
		}
    }

    public enum PAKType
    {
        PAK  = 0,
        SIN  = 1,
        HROT = 2,
    }

    public class PAK : PackFile
    {
        private protected unsafe PAK(void* handle) : base(handle) {}

        public static PAK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.PAK.Create(path);
                return handle == null ? null : new PAK(handle);
            }
        }

        public static PAK? Create(string path, PAKType type)
        {
            unsafe
            {
                var handle = Extern.PAK.Create(path, (int) type);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.PAK.Open(path, 0);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.PAK.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PAK(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.PAK.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}

		public PAKType Type
		{
			get
			{
				unsafe
				{
					return (PAKType) Extern.PAK.GetType(Handle);
				}
			}
			set
			{
			    unsafe
			    {
			        Extern.PAK.SetType(Handle, (int) value);
			    }
			}
		}
	}
}
