using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class GMA
		{
			[LibraryImport("vpkppc", EntryPoint = "vpkpp_gma_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_gma_guid")]
			public static partial sourcepp.String GUID();
		}
	}

    public class GMA : PackFile
    {
        private protected unsafe GMA(void* handle) : base(handle) {}

        public new static GMA? Open(string path)
        {
            unsafe
            {
                var handle = Extern.GMA.Open(path, 0);
                return handle == null ? null : new GMA(handle);
            }
        }

        public new static GMA? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.GMA.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new GMA(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.GMA.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
