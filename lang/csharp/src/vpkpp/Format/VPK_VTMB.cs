using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class VPK_VTMB
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_vtmb_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_vtmb_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_vpk_vtmb_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class VPK_VTMB : PackFile
    {
        private protected unsafe VPK_VTMB(void* handle) : base(handle) {}

        public static VPK_VTMB? Create(string path)
        {
            unsafe
            {
                var handle = Extern.VPK_VTMB.Create(path);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

        public new static VPK_VTMB? Open(string path)
        {
            unsafe
            {
                var handle = Extern.VPK_VTMB.Open(path, 0);
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

        public new static VPK_VTMB? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.VPK_VTMB.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new VPK_VTMB(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.VPK_VTMB.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
