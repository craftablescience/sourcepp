using System;

namespace sourcepp.steampp;

public sealed class Steam : sourcepp.ManagedNativeHandle
{
	private Steam(nint handle) : base(handle, DLL.steampp_steam_free)
	{
	}

	public Steam() : this(DLL.steampp_steam_new())
	{
	}

	public string InstallDir
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.steampp_steam_get_install_dir(Handle)).Read();
		}
	}

	public string[] LibraryDirs
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.StringArray(DLL.steampp_steam_get_library_dirs(Handle)).Read();
		}
	}

	public string SourcemodDir
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.String(DLL.steampp_steam_get_sourcemod_dir(Handle)).Read();
		}
	}

	public uint[] InstalledApps
	{
		get
		{
			ThrowIfDisposed();
			return new sourcepp.Buffer(DLL.steampp_steam_get_installed_apps(Handle)).Read<uint>();
		}
	}

	public bool IsAppInstalled(uint appId)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.steampp_steam_is_app_installed(Handle, appId));
	}

	public string AppName(uint appId)
	{
		ThrowIfDisposed();
		return DLL.steampp_steam_get_app_name(Handle, appId);
	}

	public string AppInstallDir(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_install_dir(Handle, appId)).Read();
	}
	
	public string AppIconPath(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_icon_path(Handle, appId)).Read();
	}
	
	public string AppLogoPath(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_logo_path(Handle, appId)).Read();
	}
	
	public string AppHeroPath(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_hero_path(Handle, appId)).Read();
	}
	
	public string AppBoxArtPath(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_box_art_path(Handle, appId)).Read();
	}
	
	public string AppStoreArtPath(uint appId)
	{
		ThrowIfDisposed();
		return new sourcepp.String(DLL.steampp_steam_get_app_store_art_path(Handle, appId)).Read();
	}
	
	public bool IsAppUsingGoldsrcEngine(uint appId)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.steampp_steam_is_app_using_goldsrc_engine(Handle, appId));
	}
	
	public bool IsAppUsingSourceEngine(uint appId)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.steampp_steam_is_app_using_source_engine(Handle, appId));
	}
	
	public bool IsAppUsingSource2Engine(uint appId)
	{
		ThrowIfDisposed();
		return Convert.ToBoolean(DLL.steampp_steam_is_app_using_source_2_engine(Handle, appId));
	}

	public static implicit operator bool(Steam steam)
	{
		steam.ThrowIfDisposed();
		return Convert.ToBoolean(DLL.steampp_steam_is_valid(steam.Handle));
	}
}
