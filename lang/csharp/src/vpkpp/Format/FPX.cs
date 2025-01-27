using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class FPX
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fpx_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fpx_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fpx_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class FPX : PackFile
    {
        private protected unsafe FPX(void* handle) : base(handle) {}

        public static FPX? Create(string path)
        {
            unsafe
            {
                var handle = Extern.FPX.Create(path);
                return handle == null ? null : new FPX(handle);
            }
        }

        public new static FPX? Open(string path)
        {
            unsafe
            {
                var handle = Extern.FPX.Open(path, 0);
                return handle == null ? null : new FPX(handle);
            }
        }

        public new static FPX? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.FPX.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new FPX(handle);
            }
        }

        public static string GUID
        {
            get
            {
                unsafe
                {
					var str = Extern.FPX.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
            }
        }
    }
}
