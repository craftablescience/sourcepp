using System;
using System.Runtime.InteropServices;

namespace vpkpp.Format
{
    using EntryCallback = Action<string, Entry>;

    internal static unsafe partial class Extern
    {
		internal static unsafe partial class OL
		{
			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ol_open")]
			public static partial void* Open([MarshalAs(UnmanagedType.LPStr)] string path, IntPtr callback);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ol_guid")]
			public static partial sourcepp.String GUID();

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ol_get_notes")]
			public static partial sourcepp.String GetNotes(void* handle);

			[LibraryImport("sourcepp_vpkppc", EntryPoint = "vpkpp_ol_get_entry_notes")]
			public static partial sourcepp.String GetEntryNotes(void* handle, [MarshalAs(UnmanagedType.LPStr)] string path);
		}
	}

    public class OL : PackFile
    {
        private protected unsafe OL(void* handle) : base(handle) {}

        public new static OL? Open(string path)
        {
            unsafe
            {
                var handle = Extern.OL.Open(path, 0);
                return handle == null ? null : new OL(handle);
            }
        }

        public new static OL? Open(string path, EntryCallback callback)
        {
            unsafe
            {
                EntryCallbackNative callbackNative = (path, entry) =>
                {
                    callback(path, new Entry(entry, true));
                };
                var handle = Extern.OL.Open(path, Marshal.GetFunctionPointerForDelegate(callbackNative));
                return handle == null ? null : new OL(handle);
            }
        }

		public string GetEntryNotes(string path)
		{
			unsafe
			{
				var str = Extern.OL.GetEntryNotes(Handle, path);
            	return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
			}
		}

		public string Notes
		{
			get
			{
				unsafe
				{
					var str = Extern.OL.GetNotes(Handle);
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}

		public static string GUID
		{
			get
			{
				unsafe
				{
					var str = Extern.OL.GUID();
					return sourcepp.StringUtils.ConvertToStringAndDelete(ref str);
				}
			}
		}
	}
}
