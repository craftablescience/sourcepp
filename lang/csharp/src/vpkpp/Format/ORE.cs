using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class ORE
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ore_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ore_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ore_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class ORE : PackFile
    {
        private protected unsafe ORE(void* handle) : base(handle) {}

        public static ORE? Create(string path)
        {
            unsafe
            {
                var handle = Extern.ORE.Create(path);
                return handle == null ? null : new ORE(handle);
            }
        }

        public new static ORE? Open(string path)
        {
            unsafe
            {
                var handle = Extern.ORE.Open(path, 0);
                return handle == null ? null : new ORE(handle);
            }
        }

        public new static ORE? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.ORE.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new ORE(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.ORE.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
