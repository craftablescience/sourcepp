using System;

namespace sourcepp.sourcepp;

public abstract class ManagedNativeObject : IDisposable
{
	private bool _disposed;

	~ManagedNativeObject() => Dispose();

	protected abstract void DisposeCallback();
	
	public void Dispose()
	{
		if (!_disposed)
		{
			DisposeCallback();
			_disposed = true;
		}

		GC.SuppressFinalize(this);
	}

	protected void ThrowIfDisposed() => ObjectDisposedException.ThrowIf(_disposed, this);
}

public class ManagedNativeHandle : ManagedNativeObject
{
	protected delegate void DisposeNative(ref nint value);

	protected nint Handle;
	protected readonly DisposeNative DisposeNativeFunc;
	public readonly bool Managed;

	protected ManagedNativeHandle(nint handle, DisposeNative disposeNativeFunc, bool managed = false)
	{
		Handle = handle;
		DisposeNativeFunc = disposeNativeFunc;
		Managed = managed;
	}

	protected override void DisposeCallback()
	{
		if (Managed)
		{
			DisposeNativeFunc(ref Handle);
		}
	}
}
