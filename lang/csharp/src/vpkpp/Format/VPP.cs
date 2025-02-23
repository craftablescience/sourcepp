using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class VPP
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpp_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpp_guid")]
			public static partial sourcepp.String GUID();
		}
	}

    public class VPP : PackFile
    {
        private protected unsafe VPP(void* handle) : base(handle) {}

        public new static VPP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.VPP.Open(path, 0);
                return handle == null ? null : new VPP(handle);
            }
        }

        public new static VPP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.VPP.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new VPP(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.VPP.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
