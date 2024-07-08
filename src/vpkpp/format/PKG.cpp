#include <vpkpp/format/PKG.h>

#include <cstddef>
#include <filesystem>

#include <FileStream.h>
#include <GDeflate.h>

#include <sourcepp/crypto/MD5.h>
#include <sourcepp/fs/FS.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/string/String.h>

using namespace sourcepp;
using namespace vpkpp;

PKG::PKG(const std::string& fullFilePath_, PackFileOptions options_)
		: PackFileReadOnly(fullFilePath_, options_) {
	this->type = PackFileType::PKG;
}

std::unique_ptr<PackFile> PKG::open(const std::string& path, PackFileOptions options, const Callback& callback) {
	auto* pkg = new PKG{path, options};
	auto packFile = std::unique_ptr<PackFile>(pkg);
	if (auto filestem = pkg->getFilestem(); filestem.length() >= 2 && parser::text::isNumber(filestem.at(filestem.length() - 2)) && parser::text::isNumber(filestem.at(filestem.length() - 1))) {
		for (int i = 0; true; i++) {
			auto numberedPath = pkg->getTruncatedFilepath() + "_" + string::padNumber(i, 2) + PKG_EXTENSION.data();
			if (!std::filesystem::exists(numberedPath)) {
				break;
			}
			PKG::openNumbered(pkg, i, numberedPath, callback);
		}
	} else {
		// Non-numbered fallback
		PKG::openNumbered(pkg, ~0, pkg->getFilepath(), callback);
	}
	return packFile;
}

void PKG::openNumbered(PKG* pkg, uint16_t index, std::string_view path, const Callback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return;
	}

	FileStream reader{std::string{path}};
	reader.seek_in(0);

	pkg->files[index] = {};
	auto* file = &pkg->files[index];

	reader.read(file->header);
	if (file->header.signature != PKG_SIGNATURE) {
		// File is not a PKG
		return;
	}
	if (file->header.version != 1) {
		// We only support v1 right now, since that's the only known version
		return;
	}

	reader.seek_in_u(file->header.treeOffset);

	auto assetCount = reader.read<uint16_t>();
	auto blobCount = reader.read<uint16_t>();

	reader.read(file->assets, assetCount);
	reader.read(file->blobs, blobCount);

	std::vector<std::string> names;
	for (uint32_t i = 0; i < assetCount; i++) {
		auto filepath = reader.read_string();
		string::normalizeSlashes(filepath);
		names.push_back(std::move(filepath));
	}

	for (uint32_t i = 0; i < assetCount; i++) {
		auto filepath = names[file->assets[i].nameIdx];
		auto [dir, name] = splitFilenameAndParentDir(filepath);
		if (!pkg->entries.contains(dir)) {
			pkg->entries[dir] = {};
		}

		Entry entry = createNewEntry();
		entry.path = filepath;
		{
			BufferStream offset{&entry.offset, sizeof(entry.offset)};
			offset << i << file->assets[i].baseBlobIdx << file->assets[i].tailBlobIdx;
		}
		entry.length = 144; // DDS header size + DX10 extension
		for (int j = file->assets[i].baseBlobIdx; j <= file->assets[i].tailBlobIdx; j++) {
			entry.length += file->blobs[i].size;
		}
		entry.archiveIndex = index;

		pkg->entries[dir].push_back(entry);

		if (callback) {
			callback(dir, entry);
		}
	}
}

std::optional<std::vector<std::byte>> PKG::readEntry(const Entry& entry) const {
	if (entry.unbaked) {
		// Get the stored data
		for (const auto& [unbakedEntryDir, unbakedEntryList] : this->unbakedEntries) {
			for (const Entry& unbakedEntry : unbakedEntryList) {
				if (unbakedEntry.path == entry.path) {
					std::vector<std::byte> unbakedData;
					if (isEntryUnbakedUsingByteBuffer(unbakedEntry)) {
						unbakedData = std::get<std::vector<std::byte>>(getEntryUnbakedData(unbakedEntry));
					}
					else {
						unbakedData = fs::readFileBuffer(std::get<std::string>(getEntryUnbakedData(unbakedEntry)));
					}
					return unbakedData;
				}
			}
		}
		return std::nullopt;
	}

	FileStream stream{this->getTruncatedFilepath() + "_" + string::padNumber(entry.archiveIndex, 2) + PKG_EXTENSION.data()};
	if (!stream) {
		return std::nullopt;
	}

	uint32_t assetIndex;
	uint16_t blobStart;
	uint16_t blobEnd;
	{
		BufferStreamReadOnly offset{&entry.offset, sizeof(entry.offset)};
		offset >> assetIndex >> blobStart >> blobEnd;
	}

	const auto& file = this->files.at(entry.archiveIndex);

	std::vector<std::byte> out;

	// Write DDS header with DX10 extension
	//struct DDS_HEADER
	//{
	//	uint32_t        size;
	//	uint32_t        flags;
	//	uint32_t        height;
	//	uint32_t        width;
	//	uint32_t        pitchOrLinearSize;
	//	uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
	//	uint32_t        mipMapCount;
	//	uint32_t        reserved1[11];
	//	DDS_PIXELFORMAT ddspf;
	//	uint32_t        caps;
	//	uint32_t        caps2;
	//	uint32_t        caps3;
	//	uint32_t        caps4;
	//	uint32_t        reserved2;
	//};
	//struct DDS_HEADER_DXT10
	//{
	//	DXGI_FORMAT     dxgiFormat;
	//	uint32_t        resourceDimension;
	//	uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
	//	uint32_t        arraySize;
	//	uint32_t        miscFlags2; // see DDS_MISC_FLAGS2
	//};
	{
		const auto& asset = file.assets[assetIndex];
		BufferStream writer{out};

		// DDS header
		writer.write<uint32_t>(0x20534444); // signature ("DDS ")
		writer.write<uint32_t>(124);
		writer.write<uint32_t>(0); // todo: flags
		writer.write<uint32_t>(asset.height);
		writer.write<uint32_t>(asset.width);
		writer.write<uint32_t>(0); // todo: pitchOrLinearSize
		writer.write<uint32_t>(asset.depth);
		writer.write<uint32_t>(asset.numMips); // might need to get rid of / copy in numTailMips?
		writer.write<std::array<uint32_t, 11>>({}); // reserved
		writer.write<uint32_t>(32); // todo: pixel format
		writer.write<uint32_t>(0); // todo: pixel format flags
		writer.write<uint32_t>('1TXD'); // todo: pixel format fourCC
		writer.write<uint32_t>(0); // todo: pixel format rgb bit count
		writer.write<uint32_t>(0); // todo: pixel format rgba total bits
		writer.write<uint32_t>(0); // todo: pixel format r bitmask
		writer.write<uint32_t>(0); // todo: pixel format g bitmask
		writer.write<uint32_t>(0); // todo: pixel format b bitmask
		writer.write<uint32_t>(0); // todo: pixel format a bitmask
		writer.write<uint32_t>(0); // todo: caps
		writer.write<uint32_t>(0); // todo: caps
		writer.write<uint32_t>(0); // todo: caps
		writer.write<uint32_t>(0); // todo: caps
		writer.write<uint32_t>(0); // reserved

		out.resize(writer.size());
	}

	// Add blobs
	for (auto i = blobStart; i <= blobEnd; i++) {
		const auto& blob = file.blobs[i];
		if (blob.compression != 0) {
			auto blobCompressed = stream.seek_in_u(blob.offset).read_bytes(blob.size);
			auto blobDecompressed = GDeflate::Decompress(reinterpret_cast<const uint8_t*>(blobCompressed.data()), blobCompressed.size(), 1);
			if (!blobDecompressed) {
				return std::nullopt;
			}
			for (auto byte : *blobDecompressed) {
				out.push_back(static_cast<std::byte>(byte));
			}
		} else {
			stream.seek_in_u(blob.offset).read(out, blob.size);
		}
	}

	return out;
}

std::string PKG::getTruncatedFilestem() const {
	std::string filestem = this->getFilestem();
	if (filestem.length() >= 3) {
		filestem = filestem.substr(0, filestem.length() - 3);
	}
	return filestem;
}

std::vector<Attribute> PKG::getSupportedEntryAttributes() const {
	using enum Attribute;
	return {LENGTH, ARCHIVE_INDEX};
}

const std::unordered_map<uint16_t, PKG::File>& PKG::getRawFileData() const {
	return this->files;
}
