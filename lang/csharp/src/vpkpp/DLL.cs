using System;
using System.Runtime.InteropServices;
using sourcepp.vpkpp.Format;

namespace sourcepp.vpkpp;

internal static partial class DLL
{
	private const string Name = "sourcepp_vpkppc";

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_apk_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_apk_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_fgp_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_fgp_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_fgp_get_loading_screen_file_path(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vpkpp_fgp_set_loading_screen_file_path(nint handle, string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial uint vpkpp_fgp_hash_file_path(string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_fpx_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_fpx_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_gcf_open(string path, EntryCallbackNative? callback, OpenPropertyRequestNative? requestProperty);
	
	[LibraryImport(Name)]
	public static partial uint vpkpp_gcf_get_version(nint handle);
	
	[LibraryImport(Name)]
	public static partial uint vpkpp_gcf_get_appid(nint handle);
	
	[LibraryImport(Name)]
	public static partial uint vpkpp_gcf_get_app_version(nint handle);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_gma_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_hog_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_ol_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_ol_get_notes(nint handle);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.String vpkpp_ol_get_entry_notes(nint handle, string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_007_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_ore_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_ore_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pak_create(string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pak_create_with_options(string path, PAKType type);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pak_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name)]
	public static partial PAKType vpkpp_pak_get_type(nint handle);
	
	[LibraryImport(Name)]
	public static partial void vpkpp_pak_set_type(nint handle, PAKType type);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pck_create(string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pck_create_with_options(string path, uint version, uint godotMajorVersion, uint godotMinorVersion, uint godotPatchVersion);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pck_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpk_create(string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpk_create_with_options(string path, uint version);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpk_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_vpk_generate_keypair_files(string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_vpk_sign_from_file(nint handle, string filename);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_vpk_sign_from_mem(nint handle, ReadOnlySpan<byte> privateKeyBuffer, ulong privateKeyLen, ReadOnlySpan<byte> publicKeyBuffer, ulong publicKeyLen);

	[LibraryImport(Name)]
	public static partial uint vpkpp_vpk_get_version(nint handle);
	
	[LibraryImport(Name)]
	public static partial void vpkpp_vpk_set_version(nint handle, uint version);

	[LibraryImport(Name)]
	public static partial uint vpkpp_vpk_get_chunk_size(nint handle);
	
	[LibraryImport(Name)]
	public static partial void vpkpp_vpk_set_chunk_size(nint handle, uint version);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpk_vtmb_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpk_vtmb_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_vpp_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_wad3_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_wad3_open(string path, EntryCallbackNative? callback);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_xzp_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_zip_create(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_zip_open(string path, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial EntryCompressionType vpkpp_zip_get_entry_compression_type(nint handle, string path);
	
	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vpkpp_zip_set_entry_compression_type(nint handle, string path, EntryCompressionType type);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial short vpkpp_zip_get_entry_compression_strength(nint handle, string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vpkpp_zip_set_entry_compression_strength(nint handle, string path, short strength);

	[LibraryImport(Name)]
	public static partial uint vpkpp_entry_get_flags(nint handle);

	[LibraryImport(Name)]
	public static partial uint vpkpp_entry_get_archive_index(nint handle);

	[LibraryImport(Name)]
	public static partial ulong vpkpp_entry_get_length(nint handle);

	[LibraryImport(Name)]
	public static partial ulong vpkpp_entry_get_compressed_length(nint handle);

	[LibraryImport(Name)]
	public static partial ulong vpkpp_entry_get_offset(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.Buffer vpkpp_entry_get_extra_data(nint handle);

	[LibraryImport(Name)]
	public static partial uint vpkpp_entry_get_crc32(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_entry_is_unbaked(nint handle);

	[LibraryImport(Name)]
	public static partial void vpkpp_entry_close(ref nint handle);

	internal delegate sourcepp.DLL.Buffer OpenPropertyRequestNative(nint handle, OpenProperty property);

	internal delegate void EntryCallbackNative(string path, nint entry);

	internal delegate int EntryPredicateNative(string path, nint entry);

	internal delegate EntryOptions EntryCreationNative(string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pack_file_open(string path, EntryCallbackNative? callback, OpenPropertyRequestNative? requestProperty);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.StringArray vpkpp_pack_file_get_openable_extensions();

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_has_entry_checksums(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.StringArray vpkpp_pack_file_verify_entry_checksums(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_has_pack_file_checksum(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_verify_pack_file_checksum(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_has_pack_file_signature(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_verify_pack_file_signature(nint handle);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_is_case_sensitive(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_has_entry(nint handle, string path, int includeUnbaked);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial nint vpkpp_pack_file_find_entry(nint handle, string path, int includeUnbaked);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.Buffer vpkpp_pack_file_read_entry(nint handle, string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_read_entry_text(nint handle, string path);

	[LibraryImport(Name)]
	public static partial int vpkpp_pack_file_is_read_only(nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_add_entry_from_file(nint handle, string entryPath, string filepath);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_add_entry_from_file_with_options(nint handle, string entryPath, string filepath, EntryOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_add_entry_from_mem(nint handle, string path, ReadOnlySpan<byte> buffer, ulong bufferLen);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_add_entry_from_mem_with_options(nint handle, string path, ReadOnlySpan<byte> buffer, ulong bufferLen, EntryOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial long vpkpp_pack_file_add_directory(nint handle, string entryBaseDir, string dir, EntryCreationNative? creation);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial long vpkpp_pack_file_add_directory_with_options(nint handle, string entryBaseDir, string dir, EntryOptions options);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_rename_entry(nint handle, string oldPath, string newPath);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_rename_directory(nint handle, string oldDir, string newDir);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_remove_entry(nint handle, string path);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial long vpkpp_pack_file_remove_directory(nint handle, string dirName);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_bake(nint handle, string outputDir, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_bake_with_options(nint handle, string outputDir, BakeOptions options, EntryCallbackNative? callback);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_extract_entry(nint handle, string entryPath, string filePath);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_extract_directory(nint handle, string dir, string outputDir);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_extract_all(nint handle, string outputDir, int createUnderPackFileDir);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial int vpkpp_pack_file_extract_all_if(nint handle, string outputDir, EntryPredicateNative predicate, int stripSharedDirs);

	[LibraryImport(Name)]
	public static partial ulong vpkpp_pack_file_get_entry_count(nint handle, int includeUnbaked);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vpkpp_pack_file_run_for_all_entries(nint handle, EntryCallbackNative operation, int includeUnbaked);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial void vpkpp_pack_file_run_for_all_entries_under(nint handle, string parentDir, EntryCallbackNative operation, int recursive, int includeUnbaked);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_filepath(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_truncated_filepath(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_filename(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_truncated_filename(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_filestem(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_get_truncated_filestem(nint handle);

	[LibraryImport(Name)]
	public static partial Attribute vpkpp_pack_file_get_supported_entry_attributes(nint handle);

	[LibraryImport(Name)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_to_string(nint handle);

	[LibraryImport(Name)]
	public static partial void vpkpp_pack_file_close(ref nint handle);

	[LibraryImport(Name, StringMarshalling = StringMarshalling.Utf8)]
	public static partial sourcepp.DLL.String vpkpp_pack_file_escape_entry_path_for_write(string path);
}
