using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace sourcepp.sourcepp;

internal sealed class String(DLL.String str) : ManagedNativeObject
{
	private DLL.String _str = str;

	protected override void DisposeCallback()
	{
		DLL.sourcepp_string_free(ref _str);
	}
	
	public bool IsValid
	{
		get
		{
			ThrowIfDisposed();
			return _str.Data != nint.Zero || _str.Size == 0;
		}
	}

	public string Read()
	{
		ThrowIfDisposed();
		return Marshal.PtrToStringUTF8(_str.Data, (int) _str.Size);
	}
}

internal sealed class StringArray(DLL.StringArray arr) : ManagedNativeObject
{
	private DLL.StringArray _arr = arr;

	protected override void DisposeCallback()
	{
		DLL.sourcepp_string_array_free(ref _arr);
	}
	
	public bool IsValid
	{
		get
		{
			ThrowIfDisposed();
			unsafe
			{
				return _arr.Data is not null || _arr.Size == 0;
			}
		}
	}

	public string[] Read()
	{
		ThrowIfDisposed();
		var strings = new string[_arr.Size];
		for (long i = 0; i < _arr.Size; i++)
		{
			unsafe
			{
				strings[i] = Marshal.PtrToStringUTF8(_arr.Data[i]) ?? "";
			}
		}
		return strings;
	}
}
