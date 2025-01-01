using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class OO7
		{
			[LibraryImport("vpkppc", EntryPoint = "vpkpp_007_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_007_guid")]
			public static partial sourcepp.String GUID();
		}
	}

    public class OO7 : PackFile
    {
        private protected unsafe OO7(void* handle) : base(handle) {}

        public new static OO7? Open(string path)
        {
            unsafe
            {
                var handle = Extern.OO7.Open(path, 0);
                return handle == null ? null : new OO7(handle);
            }
        }

        public new static OO7? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.OO7.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new OO7(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.OO7.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
