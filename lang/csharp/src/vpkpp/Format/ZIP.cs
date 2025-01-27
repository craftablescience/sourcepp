using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class ZIP
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_create")]
			public static partial void* Create([MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_guid")]
			public static partial sourcepp.String GUID();

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_get_entry_compression_type")]
			public static partial int GetEntryCompressionType(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_set_entry_compression_type")]
			public static partial void SetEntryCompressionType(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, int type);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_get_entry_compression_strength")]
			public static partial short GetEntryCompressionStrength(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_zip_set_entry_compression_strength")]
			public static partial void SetEntryCompressionStrength(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path, short strength);
		}
	}

    public class ZIP : PackFile
    {
        private protected unsafe ZIP(void* handle) : base(handle) {}

        public static ZIP? Create(string path)
        {
            unsafe
            {
                var handle = Extern.ZIP.Create(path);
                return handle == null ? null : new ZIP(handle);
            }
        }

        public new static ZIP? Open(string path)
        {
            unsafe
            {
                var handle = Extern.ZIP.Open(path, 0);
                return handle == null ? null : new ZIP(handle);
            }
        }

        public new static ZIP? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.ZIP.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new ZIP(handle);
            }
        }

		public EntryCompressionType GetEntryCompressionType(string path)
		{
			unsafe
			{
				return (EntryCompressionType)Extern.ZIP.GetEntryCompressionType(Handle, path);
			}
		}

		public void SetEntryCompressionType(string path, EntryCompressionType type)
		{
			unsafe
			{
				Extern.ZIP.SetEntryCompressionType(Handle, path, (int)type);
			}
		}

		public short GetEntryCompressionStrength(string path)
		{
			unsafe
			{
				return Extern.ZIP.GetEntryCompressionStrength(Handle, path);
			}
		}

		public void SetEntryCompressionStrength(string path, short strength)
		{
			unsafe
			{
				Extern.ZIP.SetEntryCompressionStrength(Handle, path, strength);
			}
		}

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.ZIP.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
