// ReSharper disable CppLocalVariableMayBeConst

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/trampoline.h>

namespace py = nanobind;
using namespace py::literals;

#include <vpkpp/vpkpp.h>

namespace vpkpp {

class PackFileTrampoline : public PackFile {
public:
	NB_TRAMPOLINE(PackFile, 18);

	[[nodiscard]] std::string_view getGUID() const override { NB_OVERRIDE_PURE_NAME("guid", getGUID); }
	[[nodiscard]] bool hasEntryChecksums() const override { NB_OVERRIDE_NAME("has_entry_checksums", hasEntryChecksums); }
	[[nodiscard]] std::vector<std::string> verifyEntryChecksums() const override { NB_OVERRIDE_NAME("verify_entry_checksums", verifyEntryChecksums); }
	[[nodiscard]] bool hasPackFileChecksum() const override { NB_OVERRIDE_NAME("has_pack_file_checksum", hasPackFileChecksum); }
	[[nodiscard]] bool verifyPackFileChecksum() const override { NB_OVERRIDE_NAME("verify_pack_file_checksum", verifyPackFileChecksum); }
	[[nodiscard]] bool hasPackFileSignature() const override { NB_OVERRIDE_NAME("has_pack_file_signature", hasPackFileSignature); }
	[[nodiscard]] bool verifyPackFileSignature() const override { NB_OVERRIDE_NAME("verify_pack_file_signature", verifyPackFileSignature); }
	[[nodiscard]] bool isCaseSensitive() const override { NB_OVERRIDE_NAME("is_case_sensitive", isCaseSensitive); }
	[[nodiscard]] std::optional<std::vector<std::byte>> readEntry(const std::string& path) const override { NB_OVERRIDE_PURE_NAME("read_entry", readEntry, path); }
	[[nodiscard]] bool isReadOnly() const noexcept override { NB_OVERRIDE_NAME("is_read_only", isReadOnly); }
	bool renameEntry(const std::string& oldPath, const std::string& newPath) override { NB_OVERRIDE_NAME("rename_entry", renameEntry, oldPath, newPath); } // NOLINT(*-use-nodiscard)
	bool renameDirectory(const std::string& oldDir, const std::string& newDir) override { NB_OVERRIDE_NAME("rename_directory", renameDirectory, oldDir, newDir); } // NOLINT(*-use-nodiscard)
	bool removeEntry(const std::string& path) override { NB_OVERRIDE_NAME("remove_entry", removeEntry, path); }
	std::size_t removeDirectory(const std::string& dirName) override { NB_OVERRIDE_NAME("remove_directory", removeDirectory, dirName); }
	bool bake(const std::string& outputDir, BakeOptions options, const EntryCallback& callback) override { NB_OVERRIDE_PURE_NAME("bake", bake, outputDir, options, callback); }
	[[nodiscard]] std::string getTruncatedFilestem() const override { NB_OVERRIDE_NAME("truncated_filestem", getTruncatedFilestem); }
	[[nodiscard]] Attribute getSupportedEntryAttributes() const override { NB_OVERRIDE_NAME("supported_entry_attributes", getSupportedEntryAttributes); }
	[[nodiscard]] explicit operator std::string() const override { NB_OVERRIDE_NAME("__str__", operator std::string); }

protected:
	explicit PackFileTrampoline(const std::string& fullFilePath_) : PackFile(fullFilePath_) {}
};

inline void register_python(py::module_& m) {
	auto vpkpp = m.def_submodule("vpkpp");
	using namespace vpkpp;

	py::enum_<Attribute>(vpkpp, "Attribute")
		.value("NONE",               Attribute::NONE)
		.value("ARCHIVE_INDEX",      Attribute::ARCHIVE_INDEX)
		.value("LENGTH",             Attribute::LENGTH)
		.value("CRC32",              Attribute::CRC32)
		.value("PCK_MD5",            Attribute::PCK_MD5)
		.value("VPK_PRELOADED_DATA", Attribute::VPK_PRELOADED_DATA);

	py::class_<Entry>(vpkpp, "Entry")
		.def_rw("flags", &Entry::flags)
		.def_rw("archive_index", &Entry::archiveIndex)
		.def_rw("length", &Entry::length)
		.def_rw("compressed_length", &Entry::compressedLength)
		.def_rw("offset", &Entry::offset)
		.def_prop_rw("extra_data", [](const Entry& self) {
			return py::bytes{self.extraData.data(), self.extraData.size()};
		}, [](Entry& self, const py::bytes& extraData) {
			self.extraData = {static_cast<const std::byte*>(extraData.data()), static_cast<const std::byte*>(extraData.data()) + extraData.size()};
		})
		.def_rw("crc32", &Entry::crc32)
		.def_rw("unbaked", &Entry::unbaked);

	py::enum_<EntryCompressionType>(vpkpp, "EntryCompressionType")
		.value("NO_OVERRIDE", EntryCompressionType::NO_OVERRIDE)
		.value("NO_COMPRESS", EntryCompressionType::NO_COMPRESS)
		.value("DEFLATE",     EntryCompressionType::DEFLATE)
		.value("BZIP2",       EntryCompressionType::BZIP2)
		.value("LZMA",        EntryCompressionType::LZMA)
		.value("ZSTD",        EntryCompressionType::ZSTD)
		.value("XZ",          EntryCompressionType::XZ);

	py::class_<BakeOptions>(vpkpp, "BakeOptions")
		.def(py::init())
		.def_rw("zip_compression_type_override", &BakeOptions::zip_compressionTypeOverride)
		.def_rw("zip_compression_strength", &BakeOptions::zip_compressionStrength)
		.def_rw("gma_write_crcs", &BakeOptions::gma_writeCRCs)
		.def_rw("vpk_generate_md5_entries", &BakeOptions::vpk_generateMD5Entries)
		.def_rw("vpk_use_buggy_extension_handling", &BakeOptions::vpk_useBuggyExtensionHandling);

	py::class_<EntryOptions>(vpkpp, "EntryOptions")
		.def(py::init())
		.def_rw("zip_compression_type", &EntryOptions::zip_compressionType)
		.def_rw("zip_compression_strength", &EntryOptions::zip_compressionStrength)
		.def_rw("vpk_preload_bytes", &EntryOptions::vpk_preloadBytes)
		.def_rw("vpk_save_to_directory", &EntryOptions::vpk_saveToDirectory);

	vpkpp.attr("EXECUTABLE_EXTENSION0") = EXECUTABLE_EXTENSION0;
	vpkpp.attr("EXECUTABLE_EXTENSION1") = EXECUTABLE_EXTENSION1;
	vpkpp.attr("EXECUTABLE_EXTENSION2") = EXECUTABLE_EXTENSION2;

	auto cPackFile = py::class_<PackFile, PackFileTrampoline>(vpkpp, "PackFile");

	py::enum_<PackFile::OpenProperty>(cPackFile, "OpenProperty", py::is_flag())
		.value("DECRYPTION_KEY", PackFile::OpenProperty::DECRYPTION_KEY);

	cPackFile
		.def_static("open", &PackFile::open, "path"_a, "callback"_a = nullptr, "request_callback"_a = nullptr)
		.def_static("get_openable_extensions", &PackFile::getOpenableExtensions)
		.def_prop_ro("guid", &PackFile::getGUID)
		.def_prop_ro("has_entry_checksums", &PackFile::hasEntryChecksums)
		.def("verify_entry_checksums", &PackFile::verifyEntryChecksums)
		.def_prop_ro("has_pack_file_checksum", &PackFile::hasPackFileChecksum)
		.def("verify_pack_file_checksum", &PackFile::verifyPackFileChecksum)
		.def_prop_ro("has_pack_file_signature", &PackFile::hasPackFileSignature)
		.def("verify_pack_file_signature", &PackFile::verifyPackFileSignature)
		.def_prop_ro("is_case_sensitive", &PackFile::isCaseSensitive)
		.def("has_entry", &PackFile::hasEntry, "path"_a, "include_unbaked"_a = true)
		.def("__contains__", [](const PackFile& self, const std::string& path) { return self.hasEntry(path); }, "path"_a)
		.def("find_entry", &PackFile::findEntry, "path"_a, "include_unbaked"_a = true)
		.def("read_entry", [](const PackFile& self, const std::string& path) -> std::optional<py::bytes> {
			const auto d = self.readEntry(path);
			if (!d) return std::nullopt;
			return py::bytes{d->data(), d->size()};
		}, "path"_a)
		.def("__getitem__", [](const PackFile& self, const std::string& path) -> std::optional<py::bytes> {
			const auto d = self[path];
			if (!d) return std::nullopt;
			return py::bytes{d->data(), d->size()};
		}, "path"_a)
		.def("read_entry_text", &PackFile::readEntryText, "path"_a)
		.def_prop_ro("is_read_only", &PackFile::isReadOnly)
		.def("add_entry_from_file", py::overload_cast<const std::string&, const std::string&, EntryOptions>(&PackFile::addEntry), "entry_path"_a, "filepath"_a, "options"_a = EntryOptions{})
		.def("add_entry_from_mem", [](PackFile& self, const std::string& entryPath, const py::bytes& buffer, EntryOptions options = {}) {
			return self.addEntry(entryPath, {static_cast<const std::byte*>(buffer.data()), buffer.size()}, options);
		}, "entry_path"_a, "buffer"_a, "options"_a = EntryOptions{})
		.def("add_directory", py::overload_cast<const std::string&, const std::string&, EntryOptions>(&PackFile::addDirectory), "entry_base_dir"_a, "dir"_a, "options"_a = EntryOptions{})
		.def("add_directory", py::overload_cast<const std::string&, const std::string&, const PackFile::EntryCreation&>(&PackFile::addDirectory), "entry_base_dir"_a, "dir"_a, "creation"_a)
		.def("rename_entry", &PackFile::renameEntry, "old_path"_a, "new_path"_a)
		.def("rename_directory", &PackFile::renameDirectory, "old_dir"_a, "new_dir"_a)
		.def("remove_entry", &PackFile::removeEntry, "path"_a)
		.def("__delitem__", &PackFile::removeEntry, "path"_a)
		.def("remove_directory", &PackFile::removeDirectory, "dir"_a)
		.def("bake", &PackFile::bake, "output_dir"_a = "", "bake_options"_a = BakeOptions{}, "callback"_a = nullptr)
		.def("extract_entry", &PackFile::extractEntry, "entry_path"_a, "filepath"_a)
		.def("extract_directory", &PackFile::extractDirectory, "dir"_a, "output_dir"_a)
		.def("extract_all", py::overload_cast<const std::string&, bool>(&PackFile::extractAll, py::const_), "output_dir"_a, "create_under_pack_file_dir"_a = true)
		.def("extract_all_if", py::overload_cast<const std::string&, const PackFile::EntryPredicate&, bool>(&PackFile::extractAll, py::const_), "output_dir"_a, "predicate"_a, "strip_shared_dirs"_a = true)
		// Skipping getBakedEntries and getUnbakedEntries
		.def("get_entry_count", &PackFile::getEntryCount, "include_unbaked"_a = true)
		.def("run_for_all_entries", py::overload_cast<const PackFile::EntryCallback&, bool>(&PackFile::runForAllEntries, py::const_), "operation"_a, "include_unbaked"_a = true)
		.def("run_for_all_entries_under", py::overload_cast<const std::string&, const PackFile::EntryCallback&, bool, bool>(&PackFile::runForAllEntries, py::const_), "parent_dir"_a, "operation"_a, "recursive"_a = true, "include_unbaked"_a = true)
		.def_prop_ro("filepath", &PackFile::getFilepath)
		.def_prop_ro("truncated_filepath", &PackFile::getTruncatedFilepath)
		.def_prop_ro("filename", &PackFile::getFilename)
		.def_prop_ro("truncated_filename", &PackFile::getTruncatedFilename)
		.def_prop_ro("filestem", &PackFile::getFilestem)
		.def_prop_ro("truncated_filestem", &PackFile::getTruncatedFilestem)
		.def_prop_ro("supported_entry_attributes", &PackFile::getSupportedEntryAttributes)
		.def("__str__", &PackFile::operator std::string)
		.def_static("escape_entry_path_for_write", &PackFile::escapeEntryPathForWrite);

	py::class_<PackFileReadOnly, PackFile>(vpkpp, "PackFileReadOnly")
		.def_prop_ro("is_read_only", &PackFileReadOnly::isReadOnly)
		.def("__str__", &PackFileReadOnly::operator std::string);

	vpkpp.attr("FPX_SIGNATURE") = FPX_SIGNATURE;
	vpkpp.attr("FPX_DIR_SUFFIX") = FPX_DIR_SUFFIX;
	vpkpp.attr("FPX_EXTENSION") = FPX_EXTENSION;

	auto cVPK = py::class_<VPK, PackFile>(vpkpp, "VPK");

	py::class_<FPX, VPK>(vpkpp, "FPX")
		.def_static("create", &FPX::create, "path"_a)
		.def_static("open", &FPX::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &FPX::GUID);

	vpkpp.attr("GCF_EXTENSION") = GCF_EXTENSION;

	py::class_<GCF, PackFileReadOnly>(vpkpp, "GCF")
		.def_static("open", &GCF::open, "path"_a, "callback"_a = nullptr, "request_callback"_a = nullptr)
		.def_ro_static("GUID", &GCF::GUID)
		.def_prop_ro("version", &GCF::getVersion)
		.def_prop_ro("appid", &GCF::getAppID)
		.def_prop_ro("appversion", &GCF::getAppVersion);

	vpkpp.attr("GMA_SIGNATURE") = GMA_SIGNATURE;
	vpkpp.attr("GMA_EXTENSION") = GMA_EXTENSION;

	py::class_<GMA, PackFile>(vpkpp, "GMA")
		.def_static("open", &GMA::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &GMA::GUID);

	vpkpp.attr("HOG_SIGNATURE") = HOG_SIGNATURE;
	vpkpp.attr("HOG_EXTENSION") = HOG_EXTENSION;

	py::class_<HOG, PackFileReadOnly>(vpkpp, "HOG")
		.def_static("open", &HOG::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &HOG::GUID);

	vpkpp.attr("OL_SIGNATURE") = OL_SIGNATURE;
	vpkpp.attr("OL_EXTENSION") = OL_EXTENSION;

	py::class_<OL, PackFileReadOnly>(vpkpp, "OL")
		.def_static("open", &OL::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &OL::GUID)
		.def_prop_ro("notes", &OL::getNotes)
		.def("get_entry_notes", &OL::getEntryNotes, "path"_a);

	vpkpp.attr("OO7_EXTENSION") = OO7_EXTENSION;

	py::class_<OO7, PackFileReadOnly>(vpkpp, "OO7")
		.def_static("open", &OO7::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &OO7::GUID);

	vpkpp.attr("ORE_EXTENSION") = ORE_EXTENSION;

	py::class_<ORE, PackFileReadOnly>(vpkpp, "ORE")
		.def_static("create", &ORE::create, "path"_a)
		.def_static("open", &ORE::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &ORE::GUID);

	vpkpp.attr("PAK_FILENAME_MAX_SIZE") = PAK_FILENAME_MAX_SIZE;
	vpkpp.attr("PAK_SIGNATURE") = PAK_SIGNATURE;
	vpkpp.attr("PAK_SIN_FILENAME_MAX_SIZE") = PAK_SIN_FILENAME_MAX_SIZE;
	vpkpp.attr("PAK_SIN_SIGNATURE") = PAK_SIN_SIGNATURE;
	vpkpp.attr("PAK_HROT_FILENAME_MAX_SIZE") = PAK_HROT_FILENAME_MAX_SIZE;
	vpkpp.attr("PAK_HROT_SIGNATURE") = PAK_HROT_SIGNATURE;
	vpkpp.attr("PAK_EXTENSION") = PAK_EXTENSION;
	vpkpp.attr("SIN_EXTENSION") = SIN_EXTENSION;

	auto cPAK = py::class_<PAK, PackFile>(vpkpp, "PAK");

	py::enum_<PAK::Type>(cPAK, "Type")
		.value("PAK", PAK::Type::PAK)
		.value("SIN", PAK::Type::SIN)
		.value("HROT", PAK::Type::HROT);

	cPAK
		.def_static("create", &PAK::create, "path"_a, "type"_a = PAK::Type::PAK)
		.def_static("open", &PAK::open, "path"_a, "type"_a = PAK::Type::PAK)
		.def_ro_static("GUID", &PAK::GUID)
		.def_prop_rw("type", &PAK::getType, &PAK::setType);

	vpkpp.attr("PCK_SIGNATURE") = PCK_SIGNATURE;
	vpkpp.attr("PCK_PATH_PREFIX") = PCK_PATH_PREFIX;
	vpkpp.attr("PCK_EXTENSION") = PCK_EXTENSION;

	py::class_<PCK, PackFile>(vpkpp, "PCK")
		.def_static("create", &PCK::create, "path"_a, "version"_a = 2, "godot_major_version"_a = 0, "godot_minor_version"_a = 0, "godot_patch_version"_a = 0)
		.def_static("open", &PCK::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &PCK::GUID)
		.def_prop_rw("version", &PCK::getVersion, &PCK::setVersion)
		.def("get_godot_version", &PCK::getGodotVersion)
		.def("set_godot_version", &PCK::setGodotVersion, "major"_a = 0, "minor"_a = 0, "patch"_a = 0);

	vpkpp.attr("VPK_SIGNATURE") = VPK_SIGNATURE;
	vpkpp.attr("VPK_DIR_INDEX") = VPK_DIR_INDEX;
	vpkpp.attr("VPK_ENTRY_TERM") = VPK_ENTRY_TERM;
	vpkpp.attr("VPK_DIR_SUFFIX") = VPK_DIR_SUFFIX;
	vpkpp.attr("VPK_EXTENSION") = VPK_EXTENSION;
	vpkpp.attr("VPK_KEYPAIR_PUBLIC_KEY_TEMPLATE") = VPK_KEYPAIR_PUBLIC_KEY_TEMPLATE;
	vpkpp.attr("VPK_KEYPAIR_PRIVATE_KEY_TEMPLATE") = VPK_KEYPAIR_PRIVATE_KEY_TEMPLATE;
	vpkpp.attr("VPK_MAX_PRELOAD_BYTES") = VPK_MAX_PRELOAD_BYTES;
	vpkpp.attr("VPK_DEFAULT_CHUNK_SIZE") = VPK_DEFAULT_CHUNK_SIZE;

	cVPK
		.def_static("create", &VPK::create, "path"_a, "version"_a = 2)
		.def_static("open", &VPK::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &VPK::GUID)
		.def_static("generate_keypair_files", &VPK::generateKeyPairFiles, "name"_a)
		.def("sign_from_file", py::overload_cast<const std::string&>(&VPK::sign), "filename"_a)
		.def("sign_from_mem", [](VPK& self, const py::bytes& privateKey, const py::bytes& publicKey) {
			return self.sign({
				static_cast<const std::byte*>(privateKey.data()), static_cast<const std::byte*>(privateKey.data()) + privateKey.size()
			}, {
				static_cast<const std::byte*>(publicKey.data()), static_cast<const std::byte*>(publicKey.data()) + publicKey.size()
			});
		}, "private_key"_a, "public_key"_a)
		.def_prop_rw("version", &VPK::getVersion, &VPK::setVersion)
		.def_prop_rw("chunk_size", &VPK::getChunkSize, &VPK::setChunkSize);

	vpkpp.attr("VPK_VTMB_EXTENSION") = VPK_VTMB_EXTENSION;

	py::class_<VPK_VTMB, PackFile>(vpkpp, "VPK_VTMB")
		.def_static("create", &VPK_VTMB::create, "path"_a)
		.def_static("open", &VPK_VTMB::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &VPK_VTMB::GUID);

	vpkpp.attr("VPP_SIGNATURE_LIL") = VPP_SIGNATURE_LIL;
	vpkpp.attr("VPP_SIGNATURE_BIG") = VPP_SIGNATURE_BIG;
	vpkpp.attr("VPP_ALIGNMENT") = VPP_ALIGNMENT;
	vpkpp.attr("VPP_EXTENSION") = VPP_EXTENSION;
	vpkpp.attr("VPP_EXTENSION_PC") = VPP_EXTENSION_PC;
	vpkpp.attr("VPP_EXTENSION_XBOX2") = VPP_EXTENSION_XBOX2;

	auto cVPP = py::class_<VPP, PackFileReadOnly>(vpkpp, "VPP");

	py::enum_<VPP::Flags>(cVPP, "Flags", py::is_flag())
		.value("NONE", VPP::Flags::FLAG_NONE)
		.value("COMPRESSED", VPP::Flags::FLAG_COMPRESSED)
		.value("CONDENSED", VPP::Flags::FLAG_CONDENSED);

	cVPP
		.def_static("open", &VPP::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &VPP::GUID);

	vpkpp.attr("WAD3_FILENAME_MAX_SIZE") = WAD3_FILENAME_MAX_SIZE;
	vpkpp.attr("WAD3_SIGNATURE") = WAD3_SIGNATURE;
	vpkpp.attr("WAD3_EXTENSION") = WAD3_EXTENSION;

	py::class_<WAD3, PackFile>(vpkpp, "WAD3")
		.def_static("create", &WAD3::create, "path"_a)
		.def_static("open", &WAD3::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &WAD3::GUID);

	vpkpp.attr("XZP_HEADER_SIGNATURE") = XZP_HEADER_SIGNATURE;
	vpkpp.attr("XZP_FOOTER_SIGNATURE") = XZP_FOOTER_SIGNATURE;
	vpkpp.attr("XZP_EXTENSION") = XZP_EXTENSION;

	py::class_<XZP, PackFileReadOnly>(vpkpp, "XZP")
		.def_static("open", &XZP::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &XZP::GUID);

	vpkpp.attr("BEE_EXTENSION") = BEE_EXTENSION;
	vpkpp.attr("BMZ_EXTENSION") = BMZ_EXTENSION;
	vpkpp.attr("FPK_EXTENSION") = FPK_EXTENSION;
	vpkpp.attr("PK3_EXTENSION") = PK3_EXTENSION;
	vpkpp.attr("PK4_EXTENSION") = PK4_EXTENSION;
	vpkpp.attr("PKZ_EXTENSION") = PKZ_EXTENSION;
	vpkpp.attr("ZIP_EXTENSION") = ZIP_EXTENSION;

	py::class_<ZIP, PackFile>(vpkpp, "ZIP")
		.def_static("create", &ZIP::create, "path"_a)
		.def_static("open", &ZIP::open, "path"_a, "callback"_a = nullptr)
		.def_ro_static("GUID", &ZIP::GUID)
		.def("get_entry_compression_type", &ZIP::getEntryCompressionType, "path"_a)
		.def("set_entry_compression_type", &ZIP::setEntryCompressionType, "path"_a, "type"_a)
		.def("get_entry_compression_strength", &ZIP::getEntryCompressionStrength, "path"_a)
		.def("set_entry_compression_strength", &ZIP::setEntryCompressionStrength, "path"_a, "type"_a);
}

} // namespace vpkpp
