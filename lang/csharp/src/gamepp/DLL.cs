using System.Runtime.InteropServices;

namespace sourcepp.gamepp;

internal static partial class DLL
{
	private const string Name = "sourcepp_gameppc";

	[LibraryImport(Name)]
	public static partial nint gamepp_game_instance_find();

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint gamepp_game_instance_find_with_name(string windowNameOverride);

	[LibraryImport(Name)]
	public static partial void gamepp_game_instance_free(ref nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String gamepp_game_instance_get_window_title(nint handle);

	[LibraryImport(Name)]
	public static partial int gamepp_game_instance_get_window_pos_x(nint handle);

	[LibraryImport(Name)]
	public static partial int gamepp_game_instance_get_window_pos_y(nint handle);

	[LibraryImport(Name)]
	public static partial int gamepp_game_instance_get_window_width(nint handle);

	[LibraryImport(Name)]
	public static partial int gamepp_game_instance_get_window_height(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void gamepp_game_instance_command(nint handle, string command);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void gamepp_game_instance_input_begin(nint handle, string input);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void gamepp_game_instance_input_end(nint handle, string input);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void gamepp_game_instance_input_once(nint handle, string input);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void gamepp_game_instance_input_hold(nint handle, string input, double sec);

	[LibraryImport(Name)]
	public static partial void gamepp_game_instance_wait(nint handle, double sec);
}
