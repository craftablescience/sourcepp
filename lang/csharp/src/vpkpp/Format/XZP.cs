using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class XZP
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_xzp_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_xzp_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class XZP : PackFile
    {
        private protected unsafe XZP(void* handle) : base(handle) {}

        public new static XZP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.XZP.Open(path, 0);
                return handle == null ? null : new XZP(handle);
            }
        }

        public new static XZP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.XZP.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new XZP(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.XZP.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
