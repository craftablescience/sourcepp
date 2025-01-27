using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class WAD3
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_wad3_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_wad3_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_wad3_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class WAD3 : PackFile
    {
        private protected unsafe WAD3(void* handle) : base(handle) {}

        public static WAD3? Create(string path)
        {
            unsafe
            {
                var handle = Extern.WAD3.Create(path);
                return handle == null ? null : new WAD3(handle);
            }
        }

        public new static WAD3? Open(string path)
        {
            unsafe
            {
                var handle = Extern.WAD3.Open(path, 0);
                return handle == null ? null : new WAD3(handle);
            }
        }

        public new static WAD3? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.WAD3.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new WAD3(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.WAD3.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
