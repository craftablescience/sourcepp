namespace sourcepp.gamepp;

public sealed class GameInstance : sourcepp.ManagedNativeHandle
{
	private GameInstance(nint handle) : base(handle, DLL.gamepp_game_instance_free)
	{
	}

	public static GameInstance? Find(string windowNameOverride = "")
	{
		var handle = windowNameOverride.Length == 0 ? DLL.gamepp_game_instance_find() : DLL.gamepp_game_instance_find_with_name(windowNameOverride);
		return handle == nint.Zero ? null : new GameInstance(handle);
	}
		
	public string WindowTitle
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.gamepp_game_instance_get_window_title(Handle)).Read();
		}
	}

	public (int, int) WindowPos
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.gamepp_game_instance_get_window_pos_x(Handle), DLL.gamepp_game_instance_get_window_pos_y(Handle));
		}
	}

	public (int, int) WindowSize
	{
		get
		{
			ThrowIfDisposed();
			return (DLL.gamepp_game_instance_get_window_width(Handle), DLL.gamepp_game_instance_get_window_height(Handle));
		}
	}

	public void Command(string command)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_command(Handle, command);
	}

	public void InputBegin(string input)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_input_begin(Handle, input);
	}

	public void InputEnd(string input)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_input_end(Handle, input);
	}

	public void InputOnce(string input)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_input_once(Handle, input);
	}

	public void InputHold(string input, double seconds)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_input_hold(Handle, input, seconds);
	}

	public void Wait(double seconds)
	{
		ThrowIfDisposed();
		DLL.gamepp_game_instance_wait(Handle, seconds);
	}
}
