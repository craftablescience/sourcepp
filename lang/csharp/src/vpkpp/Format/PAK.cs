using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class PAK
		{
			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pak_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pak_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pak_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class PAK : PackFile
    {
        private protected unsafe PAK(void* handle) : base(handle) {}

        public static PAK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.PAK.Create(path);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.PAK.Open(path, 0);
                return handle == null ? null : new PAK(handle);
            }
        }

        public new static PAK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.PAK.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PAK(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.PAK.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
