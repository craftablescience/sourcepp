using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class HOG
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_hog_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_hog_guid")]
			public static partial sourcepp.String GUID();
		}
	}

    public class HOG : PackFile
    {
        private protected unsafe HOG(void* handle) : base(handle) {}

        public new static HOG? Open(string path)
        {
            unsafe
            {
                var handle = Extern.HOG.Open(path, 0);
                return handle == null ? null : new HOG(handle);
            }
        }

        public new static HOG? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.HOG.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new HOG(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.HOG.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
