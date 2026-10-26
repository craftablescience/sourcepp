// ReSharper disable CppRedundantQualifier

#include <cctype>

#include <vpkpp/format/PAK.h>

#include <filesystem>

#include <FileStream.h>

using namespace sourcepp;
using namespace vpkpp;

std::unique_ptr<PackFile> PAK::create(const std::string& path, Type type) {
	{
		FileStream stream{path, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		switch (type) {
			case Type::PAK:
			case Type::DAIKATANA:
				stream << PAK_SIGNATURE;
				break;
			case Type::SIN:
				stream << PAK_SIN_SIGNATURE;
				break;
			case Type::SIN_RELOADED:
				stream << PAK_SIN_RELOADED_SIGNATURE;
				break;
			case Type::HROT:
				stream << PAK_HROT_SIGNATURE;
				break;
		}
		stream
			.write<uint32_t>(0)
			.write<uint32_t>(0);
	}
	return PAK::open(path);
}

std::unique_ptr<PackFile> PAK::open(const std::string& path, const EntryCallback& callback) {
	if (!std::filesystem::exists(path)) {
		// File does not exist
		return nullptr;
	}

	auto* pak = new PAK{path};
	auto packFile = std::unique_ptr<PackFile>(pak);

	FileStream reader{pak->fullFilePath};

	// Get total file size
	reader.seek_in(0, std::ios::end);
	const auto pakFileLength = reader.tell_in();
	reader.seek_in(0);

	if (const auto signature = reader.read<uint32_t>(); signature == PAK_SIGNATURE) {
		pak->type = Type::PAK;
	} else if (signature == PAK_SIN_SIGNATURE) {
		pak->type = Type::SIN;
	} else if (signature == PAK_SIN_RELOADED_SIGNATURE) {
		pak->type = Type::SIN_RELOADED;
	} else if (signature == PAK_HROT_SIGNATURE) {
		pak->type = Type::HROT;
	} else {
		// File is not a PAK
		return nullptr;
	}

	if (pak->type == Type::SIN_RELOADED) {
		reader.skip_in<int32_t>(); // reserved
		const auto directoryOffset = reader.read<uint64_t>();
		const auto namesOffset = reader.read<uint64_t>();
		const auto fileCount = reader.read<uint32_t>();
		const auto namesLength = reader.read<uint32_t>();

		// read names buffer
		std::string namesBuffer;
		reader.seek_in_u(namesOffset);
		reader.read(namesBuffer, namesLength, false);

		// read directory
		reader.seek_in_u(directoryOffset);
		for (uint32_t i = 0; i < fileCount; i++) {
			Entry entry = createNewEntry();

			const auto filePos = reader.read<uint64_t>();
			const auto fileLen = reader.read<uint32_t>();
			const auto nameOfs = reader.read<uint32_t>();

			auto entryPath = pak->cleanEntryPath(namesBuffer.c_str() + nameOfs);

			entry.offset = filePos;
			entry.length = fileLen;

			pak->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
	} else {
		// Directory offset and size
		const auto directoryOffset = reader.read<uint32_t>();
		const auto directorySize = reader.read<uint32_t>();

		// Check if it's a Daikatana PAK
		bool isDaikatana = directorySize % 64 != 0;
		const auto entrySize = sizeof(uint32_t) * 2 + pak->getFilenameLength();
		const auto dkEntrySize = sizeof(uint32_t) * 4 + pak->getFilenameLength();

		// Check again if it's a Daikatana PAK for this edge case
		if (directorySize % 576 == 0) {
			const auto numEntries = directorySize / dkEntrySize;

			for (size_t i = 0; i < numEntries; i++) {
				size_t j;

				// check filename
				auto entryPath = pak->cleanEntryPath(reader.read_string(pak->getFilenameLength()));

				for (j = 0; j < pak->getFilenameLength(); j++) {
					if (entryPath[j] == '\0' || !std::isalnum(entryPath[i])) {
						break;
					}
				}

				if (j == 0 || !std::isalnum(entryPath[i])) {
					isDaikatana = false;
					break;
				}

				uint32_t offset = reader.read<uint32_t>();
				uint32_t length = reader.read<uint32_t>();

				// check compression
				uint32_t compressedLength = reader.read<uint32_t>();
				uint32_t compressedType = reader.read<uint32_t>();
				if (compressedType != 0 && compressedType != 1) {
					isDaikatana = false;
					break;
				}

				if (compressedType == 1) {
					length = compressedLength;
				}

				// check offset
				if (offset + length > pakFileLength) {
					isDaikatana = false;
					break;
				}
			}
		}

		// Set the type if we truly think it's a Daikatana PAK
		if (isDaikatana == true) {
			pak->type = Type::DAIKATANA;
		}

		// File count
		const auto fileCount = directorySize / (isDaikatana ? dkEntrySize : entrySize);

		reader.seek_in(directoryOffset);
		for (uint32_t i = 0; i < fileCount; i++) {
			Entry entry = createNewEntry();

			auto entryPath = pak->cleanEntryPath(reader.read_string(pak->getFilenameLength()));

			entry.offset = reader.read<uint32_t>();
			entry.length = reader.read<uint32_t>();

			// Read extra data
			if (isDaikatana) {
				uint32_t compressedLength = reader.read<uint32_t>();
				uint32_t compressedType = reader.read<uint32_t>();
				if (compressedType == 1) {
					entry.compressedLength = compressedLength;
				}
			}

			pak->entries.emplace(entryPath, entry);

			if (callback) {
				callback(entryPath, entry);
			}
		}
	}

	return packFile;
}

static std::vector<std::byte> dkDecompress(std::vector<std::byte> inputData) {
	std::vector<std::byte> outputData{};
	const uint8_t* inputDataPtr = reinterpret_cast<const uint8_t *>(inputData.data());
	size_t inputDataPos = 0;

	while (true) {
		if (inputDataPos >= inputData.size()) {
			break;
		}

		uint8_t code = inputDataPtr[inputDataPos++];

		if (code == 255) {
			// Terminator
			break;
		} else if (code < 64) {
			// Uncompressed block
			for (int i = 0; i <= code; i++) {
				outputData.push_back(std::byte(inputDataPtr[inputDataPos++]));
			}
		} else if (code < 128) {
			// Run length zero block
			for (int i = 62; i < code; i++) {
				outputData.push_back(std::byte(0));
			}
		} else if (code < 192) {
			// Run length encoded block
			uint8_t value = inputDataPtr[inputDataPos++];
			for (int i = 126; i < code; i++) {
				outputData.push_back(std::byte(value));
			}
		} else if (code < 254) {
			// Reference previous data
			uint8_t offset = inputDataPtr[inputDataPos++];
			for (int i = 190; i < code; i++) {
				outputData.push_back(outputData[outputData.size() - offset - 2]);
			}
		}
	}

	return outputData;
}

std::optional<std::vector<std::byte>> PAK::readEntry(const std::string& path_) const {
	const auto path = this->cleanEntryPath(path_);
	const auto entry = this->findEntry(path);
	if (!entry) {
		return std::nullopt;
	}
	if (entry->unbaked) {
		return readUnbakedEntry(*entry);
	}

	// It's baked into the file on disk
	FileStream stream{this->fullFilePath};
	if (!stream) {
		return std::nullopt;
	}
	stream.seek_in_u(entry->offset);

	// Check if we need to decompress the Daikatana PAK file
	if (this->type == Type::DAIKATANA && entry->compressedLength > 0) {
		return dkDecompress(stream.read_bytes(entry->compressedLength));
	}

	return stream.read_bytes(entry->length);
}

void PAK::addEntryInternal(Entry& entry, const std::string&, std::vector<std::byte>& buffer, EntryOptions) {
	entry.length = buffer.size();

	// Offset will be reset when it's baked
	entry.offset = 0;
}

bool PAK::bake(const std::string& outputDir_, BakeOptions, const EntryCallback& callback) {
	// Get the proper file output folder
	const std::string outputDir = this->getBakeOutputDir(outputDir_);
	const std::string outputPath = outputDir + '/' + this->getFilename();

	// Reconstruct data for ease of access
	std::vector<std::pair<std::string, Entry*>> entriesToBake;
	this->runForAllEntriesInternal([&entriesToBake](const std::string& path, Entry& entry) {
		entriesToBake.emplace_back(path, &entry);
	});

	// Read data before overwriting, we don't know if we're writing to ourself
	std::vector<std::byte> fileData;
	std::vector<std::byte> nameData;
	std::vector<uint32_t> nameOffsets;
	for (auto& [path, entry] : entriesToBake) {
		if (auto binData = this->readEntry(path)) {
			entry->offset = fileData.size();

			fileData.insert(fileData.end(), binData->begin(), binData->end());

			if (this->type == Type::SIN_RELOADED) {
				const auto oldSize = nameData.size();
				nameData.resize(oldSize + path.size() + 1);
				memcpy(&nameData[oldSize], path.data(), path.size());
				nameOffsets.push_back(oldSize);
			}
		} else {
			entry->offset = 0;
			entry->length = 0;
		}
	}

	{
		FileStream stream{outputPath, FileStream::OPT_TRUNCATE | FileStream::OPT_CREATE_IF_NONEXISTENT};
		stream.seek_out(0);

		// Signature
		stream.write<uint32_t>(this->getSignature());

		// Header
		if (this->type == Type::SIN_RELOADED) {
			// Reserved value
			// Basically a version identifier, but not used...?
			stream.write<int32_t>(0);

			// Offset of directory
			static constexpr auto HEADER_OFFSET = sizeof(uint32_t) * 3 + sizeof(uint64_t) * 2;
			stream.write<uint64_t>(HEADER_OFFSET + fileData.size());

			// Offset of names
			const auto numFiles = entriesToBake.size();
			const auto directorySize = (sizeof(uint64_t) + sizeof(uint32_t) * 2) * numFiles;
			stream.write<uint64_t>(HEADER_OFFSET + fileData.size() + directorySize);

			// Number of files
			stream.write<uint32_t>(numFiles);

			// Size of names
			stream.write<uint32_t>(nameData.size());

			// File data
			stream.write(fileData);

			// Directory
			size_t i = 0;
			for (const auto& [path, entry] : entriesToBake) {
				stream
					.write<uint64_t>(entry->offset + HEADER_OFFSET)
					.write<uint32_t>(entry->length)
					.write<uint32_t>(nameOffsets[i++]);

				if (callback) {
					callback(path, *entry);
				}
			}

			// Names
			stream.write(nameData);
		} else {
			// Offset and size of directory
			static constexpr auto HEADER_OFFSET = sizeof(uint32_t) * 3;
			stream
				.write<uint32_t>(HEADER_OFFSET + fileData.size())
				.write<uint32_t>(entriesToBake.size() * (sizeof(uint32_t) * 2 + this->getFilenameLength()));

			// File data
			stream.write(fileData);

			// Directory
			for (const auto& [path, entry] : entriesToBake) {
				stream
					.write(path, true, this->getFilenameLength())
					.write<uint32_t>(entry->offset + HEADER_OFFSET)
					.write<uint32_t>(entry->length);

				if (callback) {
					callback(path, *entry);
				}
			}
		}
	}

	// Clean up
	this->mergeUnbakedEntries();
	PackFile::setFullFilePath(outputDir);
	return true;
}

Attribute PAK::getSupportedEntryAttributes() const {
	using enum Attribute;
	return LENGTH;
}

PAK::Type PAK::getType() const {
	return this->type;
}

void PAK::setType(Type type_) {
	this->type = type_;
}

uint32_t PAK::getSignature() const {
	switch (this->type) {
		case Type::PAK:
		case Type::DAIKATANA:
			return PAK_SIGNATURE;
		case Type::SIN:
			return PAK_SIN_SIGNATURE;
		case Type::SIN_RELOADED:
			return PAK_SIN_RELOADED_SIGNATURE;
		case Type::HROT:
			return PAK_HROT_SIGNATURE;
	}
	return PAK_SIGNATURE;
}

uint8_t PAK::getFilenameLength() const {
	switch (this->type) {
		case Type::PAK:
		case Type::DAIKATANA:
			return PAK_FILENAME_MAX_SIZE;
		case Type::SIN:
			return PAK_SIN_FILENAME_MAX_SIZE;
		case Type::SIN_RELOADED:
			return 0; // variable
		case Type::HROT:
			return PAK_HROT_FILENAME_MAX_SIZE;
	}
	return PAK_FILENAME_MAX_SIZE;
}
