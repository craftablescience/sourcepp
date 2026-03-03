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
	internal delegate void DisposeNative(ref nint value);

	internal nint Handle;
	internal readonly DisposeNative? DisposeNativeFunc;
	public readonly bool Managed;

	internal ManagedNativeHandle(nint handle, DisposeNative? disposeNativeFunc, bool managed = true)
	{
		Handle = handle;
		DisposeNativeFunc = disposeNativeFunc;
		Managed = managed;
	}

	protected override void DisposeCallback()
	{
		if (Managed && DisposeNativeFunc is not null)
		{
			DisposeNativeFunc(ref Handle);
		}
	}
}
