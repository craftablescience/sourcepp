using System;
using System.Runtime.InteropServices;

namespace sourcepp.vtfpp;

public sealed class SHT : sourcepp.ManagedNativeHandle
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Sequence {
		[StructLayout(LayoutKind.Sequential)]
		public struct Frame {
			[StructLayout(LayoutKind.Sequential)]
			public struct Bounds {
				public float X1;
				public float Y1;
				public float X2;
				public float Y2;
			}

			public float Duration;
			public Bounds Bounds0;
			public Bounds Bounds1;
			public Bounds Bounds2;
			public Bounds Bounds3;
		}

		public uint Id;
		public int Loop;
		public uint FrameCount;
		public float DurationTotal;
	}

	internal SHT(nint handle) : base(handle, DLL.vtfpp_sht_close)
	{
	}

	public SHT() : this(DLL.vtfpp_sht_create())
	{
	}

	public SHT(ReadOnlySpan<byte> shtData) : this(DLL.vtfpp_sht_open_from_mem(shtData, (ulong) shtData.Length))
	{
	}

	public SHT(string shtPath) : this(DLL.vtfpp_sht_open_from_file(shtPath))
	{
	}
	
	public static implicit operator bool(SHT sht)
	{
		sht.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_sht_is_valid(sht.Handle));
	}

	public uint Version
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_sht_get_version(Handle);
		}
		set
		{
			ThrowIfDisposed();
			DLL.vtfpp_sht_set_version(Handle, value);
		}
	}

	public Sequence[] Sequences
	{
		get
		{
			ThrowIfDisposed();
			var sequences = new Sequence[DLL.vtfpp_sht_get_sequences_count(Handle)];
			for (var i = 0u; i < sequences.Length; i++)
			{
				sequences[i] = DLL.vtfpp_sht_get_sequence_at_index(Handle, i);
			}
			return sequences;
		}
		set
		{
			ThrowIfDisposed();
			var storedSequencesCount = DLL.vtfpp_sht_get_sequences_count(Handle);
			for (var i = 0u; i < value.Length; i++)
			{
				if (i < storedSequencesCount)
				{
					DLL.vtfpp_sht_set_sequence_at_index(Handle, i, value[i]);
				}
				else
				{
					DLL.vtfpp_sht_add_sequence(Handle, value[i]);
				}
			}
			for (var i = storedSequencesCount - value.Length - 1; i >= 0; i--)
			{
				DLL.vtfpp_sht_remove_sequence_at_index(Handle, (uint) i);
			}
		}
	}

	public Sequence SequenceWithId(uint id)
	{
		return DLL.vtfpp_sht_get_sequence_with_id(Handle, id);
	}
	
	public void SequenceWithId(uint id, Sequence sequence)
	{
		DLL.vtfpp_sht_set_sequence_with_id(Handle, id, sequence);
	}

	public void RemoveSequenceWithId(uint id)
	{
		DLL.vtfpp_sht_remove_sequence_with_id(Handle, id);
	}

	public Sequence.Frame[] Frames(uint sequenceId)
	{
		ThrowIfDisposed();
		var frames = new Sequence.Frame[SequenceWithId(sequenceId).FrameCount];
		for (var i = 0u; i < frames.Length; i++)
		{
			frames[i] = DLL.vtfpp_sht_get_frame(Handle, sequenceId, i);
		}
		return frames;
	}

	public void Frames(uint sequenceId, Sequence.Frame[] frames)
	{
		ThrowIfDisposed();
		var storedFramesCount = SequenceWithId(sequenceId).FrameCount;
		for (var i = 0u; i < frames.Length; i++)
		{
			if (i < storedFramesCount)
			{
				DLL.vtfpp_sht_set_frame(Handle, sequenceId, i, frames[i]);
			}
			else
			{
				DLL.vtfpp_sht_add_frame(Handle, sequenceId, frames[i]);
			}
		}
		for (var i = storedFramesCount - frames.Length - 1; i >= 0; i--)
		{
			DLL.vtfpp_sht_remove_frame(Handle, sequenceId, (uint) i);
		}
	}

	public byte FrameBoundsCount
	{
		get
		{
			ThrowIfDisposed();
			return DLL.vtfpp_sht_get_frame_bounds_count(Handle);
		}
	}

	public byte[] Bake()
	{
		ThrowIfDisposed();
		return new sourcepp.Buffer(DLL.vtfpp_sht_bake(Handle)).Read<byte>();
	}

	public bool Bake(string shtPath)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.vtfpp_sht_bake_to_file(Handle, shtPath));
	}
}
