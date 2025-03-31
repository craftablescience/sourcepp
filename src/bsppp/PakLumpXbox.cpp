#include <bsppp/PakLumpXbox.h>

#include <filesystem>

#include <bsppp/BSP.h>
#include <FileStream.h>
#include <sourcepp/String.h>
#include <vpkpp/format/XZP.h>

using namespace bsppp;
using namespace sourcepp;
using namespace vpkpp;

PakLumpXbox::PakLumpXbox(const std::string& fullFilePath_)
		: XZP(fullFilePath_)
		, tempPakLumpPath((std::filesystem::temp_directory_path() / (string::generateUUIDv4() + ".xzp")).string()) {}

PakLumpXbox::~PakLumpXbox() {
	std::filesystem::remove(this->tempPakLumpPath);
}

std::unique_ptr<PackFile> PakLumpXbox::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* bsp = new PakLumpXbox{path};
	auto packFile = std::unique_ptr<PackFile>(bsp);

	{
		bsppp::BSP reader{path};
		if (!reader) {
			// File failed to load, or has an invalid signature
			return nullptr;
		}

		bsp->version = reader.getVersion();
		bsp->mapRevision = reader.getMapRevision();

		if (auto xzpLump = reader.getLumpData(BSPLump::XBOX_XZIPPAKFILE); xzpLump && bsp->version == 19) {
			// Extract the paklump to a temp file
			FileStream writer{bsp->tempPakLumpPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
			writer.write(*xzpLump);
		} else {
			// No Xbox paklump
			return nullptr;
		}
	}

	const auto xzp = XZP::open(bsp->tempPakLumpPath, [bsp, &callback](const std::string& entryPath, const Entry& entry) {
		bsp->entries.emplace(entryPath, entry);

		if (callback) {
			callback(entryPath, entry);
		}
	});

	return packFile;
}

PakLumpXbox::operator std::string() const {
	return XZP::operator std::string() +
	       " | Version v" + std::to_string(this->version) +
	       " | Map Revision " + std::to_string(this->mapRevision);
}
