using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class SPK
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_spk_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_spk_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_spk_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class SPK : PackFile
    {
        private protected unsafe SPK(void* handle) : base(handle) {}

        public static SPK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.SPK.Create(path);
                return handle == null ? null : new SPK(handle);
            }
        }

        public new static SPK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.SPK.Open(path, 0);
                return handle == null ? null : new SPK(handle);
            }
        }

        public new static SPK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.SPK.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new SPK(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.SPK.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
