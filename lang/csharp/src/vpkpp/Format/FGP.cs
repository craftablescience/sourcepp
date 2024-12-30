using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class FGP
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fgp_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fgp_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_fgp_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class FGP : PackFile
    {
        private protected unsafe FGP(void* handle) : base(handle) {}

        public static FGP? Create(string path)
        {
            unsafe
            {
                var handle = Extern.FGP.Create(path);
                return handle == null ? null : new FGP(handle);
            }
        }

        public new static FGP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.FGP.Open(path, 0);
                return handle == null ? null : new FGP(handle);
            }
        }

        public new static FGP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.FGP.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new FGP(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.FGP.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
