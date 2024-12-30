using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class PCK
		{
			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pck_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pck_create_with_options")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pck_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("vpkppc", EntryPoint = "vpkpp_pck_guid")]
			public static partial sourcepp.String GUID();
		}
    }

    public class PCK : PackFile
    {
        private protected unsafe PCK(void* handle) : base(handle) {}

        public static PCK? Create(string path)
        {
            unsafe
            {
                var handle = Extern.PCK.Create(path);
                return handle == null ? null : new PCK(handle);
            }
        }

        public static PCK? Create(string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion)
        {
            unsafe
            {
                var handle = Extern.PCK.Create(path, version, godotMajorVersion, godotMinorVersion, godotPatchVersion);
                return handle == null ? null : new PCK(handle);
            }
        }

        public new static PCK? Open(string path)
        {
            unsafe
            {
                var handle = Extern.PCK.Open(path, 0);
                return handle == null ? null : new PCK(handle);
            }
        }

        public new static PCK? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.PCK.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new PCK(handle);
            }
        }

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.PCK.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
