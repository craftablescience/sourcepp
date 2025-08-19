using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class GCF
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_gcf_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback, IntPtr requestProperty);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_gcf_guid")]
			public static partial sourcepp.String GUID();
		}
	}

    public class GCF : PackFile
    {
        private protected unsafe GCF(void* handle) : base(handle) {}

        public new static GCF? Open(string path)
        {
            unsafe
            {
                var handle = Extern.GCF.Open(path, 0);
                return handle == null ? null : new GCF(handle);
            }
        }

        public new static GCF? Open(string path, EntryCallback callback, OpenPropertyRequest requestProperty)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.GCF.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative), Marshal.GetFunctionPointerForDelegate(requestProperty));
                return handle == null ? null : new GCF(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.GCF.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
