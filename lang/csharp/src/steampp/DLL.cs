using System.Runtime.InteropServices;

namespace sourcepp.steampp;

internal static partial class DLL
{
	private const string Name = "sourcepp_steamppc";

	[LibraryImport(Name)]
	public static partial nint steampp_steam_new();

	[LibraryImport(Name)]
	public static partial void steampp_steam_free(ref nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_install_dir(nint handle);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.StringArray steampp_steam_get_library_dirs(nint handle);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_sourcemod_dir(nint handle);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer steampp_steam_get_installed_apps(nint handle);

	[LibraryImport(Name)]
	public static partial int steampp_steam_is_app_installed(nint handle, uint appId);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial string steampp_steam_get_app_name(nint handle, uint appId);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_install_dir(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_icon_path(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_logo_path(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_hero_path(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_box_art_path(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String steampp_steam_get_app_store_art_path(nint handle, uint appId);

	[LibraryImport(Name)]
	public static partial int steampp_steam_is_app_using_goldsrc_engine(nint handle, uint appId);
	
	[LibraryImport(Name)]
	public static partial int steampp_steam_is_app_using_source_engine(nint handle, uint appId);

	[LibraryImport(Name)]
	public static partial int steampp_steam_is_app_using_source_2_engine(nint handle, uint appId);

	[LibraryImport(Name)]
	public static partial int steampp_steam_is_valid(nint handle);
}
