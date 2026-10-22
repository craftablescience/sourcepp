using System;

namespace sourcepp.sourcepp;

internal sealed class Buffer(DLL.Buffer buf) : ManagedNativeObject
{
	private DLL.Buffer _buf = buf;

	protected override void DisposeCallback()
	{
		DLL.sourcepp_buffer_free(ref _buf);
	}

	public bool IsValid
	{
		get
		{
			ThrowIfDisposed();
			unsafe
			{
				return _buf.Data is not null || _buf.Size == 0;
			}
		}
	}

	public Span<T> ReadSpan<T>()
	{
		ThrowIfDisposed();
		unsafe
		{
			return new Span<T>(_buf.Data, (int) _buf.Size);
		}
	}

	public T[] Read<T>()
	{
		ThrowIfDisposed();
		// The span points into native memory owned by this object. Allocating the array can trigger a GC, which would
		// finalize this object (freeing the memory) mid-copy if nothing keeps it alive until the copy is done.
		var result = ReadSpan<T>().ToArray();
		GC.KeepAlive(this);
		return result;
	}
}
