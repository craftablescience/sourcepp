// ReSharper disable CppRedundantQualifier

#include <toolpp/CmdSeq.h>

#include <cstring>

#include <FileStream.h>
#include <kvpp/kvpp.h>
#include <sourcepp/String.h>

using namespace kvpp;
using namespace sourcepp;
using namespace toolpp;

namespace {

CmdSeq::Command::Special specialCmdFromString(std::string_view specialCmd) {
	using enum CmdSeq::Command::Special;
	if (string::iequals(specialCmd, "change_dir")) {
		return CHANGE_DIRECTORY;
	}
	if (string::iequals(specialCmd, "copy_file")) {
		return COPY_FILE;
	}
	if (string::iequals(specialCmd, "delete_file")) {
		return DELETE_FILE;
	}
	if (string::iequals(specialCmd, "rename_file")) {
		return RENAME_FILE;
	}
	if (string::iequals(specialCmd, "copy_file_if_exists")) {
		return COPY_FILE_IF_EXISTS;
	}
	return NONE;
}

} // namespace

std::string CmdSeq::Command::getSpecialDisplayNameFor(Special special) {
	switch (special) {
		case Special::NONE:
			break;
		case Special::CHANGE_DIRECTORY:
			return "Change Directory";
		case Special::COPY_FILE:
			return "Copy File";
		case Special::DELETE_FILE:
			return "Delete File";
		case Special::RENAME_FILE:
			return "Rename File";
		case Special::COPY_FILE_IF_EXISTS:
			return "Copy File If It Exists";
	}
	return "None";
}

std::string CmdSeq::Command::getExecutableDisplayName() const {
	if (this->special != Command::Special::NONE) {
		return getSpecialDisplayNameFor(this->special);
	}
	return this->executable;
}

CmdSeq::CmdSeq(Type type_)
		: type(type_)
		, version(0.2f) {}

CmdSeq::CmdSeq(const std::filesystem::path& cmdSeqPath)
		: type(Type::INVALID)
		, version(0.2f) {
	{
		FileStream reader{cmdSeqPath};
		if (!reader) {
			return;
		}
		if (const auto binStr = reader.seek_in(0).read_string(10); binStr == "Worldcraft") {
			this->type = Type::BINARY;
		} else {
			auto kvStr = reader.seek_in(0).read_string(19);
			string::toLower(kvStr);
			if (kvStr == "\"command sequences\"") {
				this->type = Type::KEYVALUES_STRATA;
			} else {
				return;
			}
		}
	}
	switch (this->type) {
		using enum Type;
		case INVALID:
			break;
		case BINARY:
			this->parseBinary(cmdSeqPath);
			break;
		case KEYVALUES_STRATA:
			this->parseKeyValuesStrata(cmdSeqPath);
			break;
	}
}

CmdSeq::operator bool() const {
	return this->type != Type::INVALID;
}

CmdSeq::Type CmdSeq::getType() const {
	return this->type;
}

void CmdSeq::setType(Type type_) {
	this->type = type_;
}

float CmdSeq::getVersion() const {
	return this->version;
}

void CmdSeq::setVersion(bool isV02) {
	if (isV02) {
		this->version = 0.2f;
	} else {
		this->version = 0.1f;
	}
}

void CmdSeq::parseBinary(const std::filesystem::path& path) {
	FileStream reader{path};
	if (!reader) {
		return;
	}

	reader.seek_in(31).read(this->version);

	const auto sequenceCount = reader.read<uint32_t>();
	for (uint32_t s = 0; s < sequenceCount; s++) {
		auto& [seqName, seqCommands] = this->sequences.emplace_back();
		seqName = reader.read_string(128);

		const auto commandCount = reader.read<uint32_t>();
		for (uint32_t c = 0; c < commandCount; c++) {
			auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands.emplace_back();
			enabled = reader.read<int32_t>() & 0xFF;
			special = reader.read<Command::Special>();
			if (special == static_cast<Command::Special>(Command::SPECIAL_COPY_FILE_IF_EXISTS_ALIAS)) {
				special = Command::Special::COPY_FILE_IF_EXISTS;
			}
			executable = reader.read_string(260);
			arguments = reader.read_string(260);
			reader.skip_in<int32_t>();
			ensureFileExists = reader.read<int32_t>();
			pathToTheoreticallyExistingFile = reader.read_string(260);
			useProcessWindow = reader.read<int32_t>();
			if (version > 0.15f) {
				waitForKeypress = reader.read<int32_t>();
			}
		}
	}
}

void CmdSeq::parseKeyValuesStrata(const std::filesystem::path& path) {
	this->version = 0.2f;

	const KV1 cmdSeq{fs::readFileText(path)};
	for (const auto& kvSequence : cmdSeq["Command Sequences"].getChildren()) {
		auto& [seqName, seqCommands] = this->sequences.emplace_back();
		seqName = kvSequence.getKey();

		for (const auto& kvCommand : kvSequence.getChildren()) {
			auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands.emplace_back();
			string::toBool(kvCommand["enabled"].getValue(), enabled);
			const auto specialCmd = kvCommand["special_cmd"].getValue();
			if (parser::text::isNumber(specialCmd)) {
				string::toInt(specialCmd, reinterpret_cast<std::underlying_type_t<Command::Special>&>(special));
				if (special == Command::SPECIAL_COPY_FILE_IF_EXISTS_ALIAS) {
					special = Command::Special::COPY_FILE_IF_EXISTS;
				}
			} else {
				special = ::specialCmdFromString(specialCmd);
			}
			executable = kvCommand["run"].getValue();
			arguments = kvCommand["params"].getValue();
			string::toBool(kvCommand["ensure_check"].getValue(), ensureFileExists);
			pathToTheoreticallyExistingFile = kvCommand["ensure_fn"].getValue();
			string::toBool(kvCommand["use_process_wnd"].getValue(), useProcessWindow);
			string::toBool(kvCommand["no_wait"].getValue(), waitForKeypress);
		}
	}
}

std::vector<CmdSeq::Sequence>& CmdSeq::getSequences() {
	return this->sequences;
}

const std::vector<CmdSeq::Sequence>& CmdSeq::getSequences() const {
	return this->sequences;
}

std::vector<std::byte> CmdSeq::bakeBinary() const {
	std::vector<std::byte> out;
	BufferStream writer{out};

	writer
		.write("Worldcraft Command Sequences\r\n\x1a", 31)
		.write<float>(this->getVersion())
		.write<uint32_t>(this->getSequences().size());

	for (const auto& [seqName, seqCommands] : this->getSequences()) {
		writer
			.write(seqName, true, 128)
			.write<uint32_t>(seqCommands.size());

		for (const auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] : seqCommands) {
			writer
				.write<uint32_t>(enabled)
				.write(special)
				.write(executable, true, 260)
				.write(arguments, true, 260)
				.write<uint32_t>(true)
				.write<uint32_t>(ensureFileExists)
				.write(pathToTheoreticallyExistingFile, true, 260)
				.write<uint32_t>(useProcessWindow);

			if (this->getVersion() > 0.15f) {
				writer.write<uint32_t>(waitForKeypress);
			}
		}
	}

	out.resize(writer.size());
	return out;
}

std::vector<std::byte> CmdSeq::bakeKeyValuesStrata() const {
	KV1Writer kv;
	auto& kvFile = kv.addChild("Command Sequences");
	for (const auto& [seqName, seqCommands] : this->getSequences()) {
		auto& kvSequence = kvFile.addChild(seqName);
		for (int i = 1; i <= seqCommands.size(); i++) {
			const auto& [enabled, special, executable, arguments, ensureFileExists, pathToTheoreticallyExistingFile, useProcessWindow, waitForKeypress] = seqCommands[i - 1];
			auto& kvCommand = kvSequence.addChild(std::to_string(i));
			kvCommand["enabled"] = enabled;
			kvCommand["special_cmd"] = static_cast<int>(special);
			kvCommand["run"] = executable;
			kvCommand["params"] = arguments;
			kvCommand["ensure_check"] = ensureFileExists;
			kvCommand["ensure_fn"] = pathToTheoreticallyExistingFile;
			kvCommand["use_process_wnd"] = useProcessWindow;
			kvCommand["no_wait"] = waitForKeypress;
		}
	}

	const auto kvStr = kv.bake();
	std::vector<std::byte> out;
	out.resize(kvStr.length());
	std::memcpy(out.data(), kvStr.data(), kvStr.length());
	return out;
}

std::vector<std::byte> CmdSeq::bake() const {
	switch (this->type) {
		using enum Type;
		case INVALID:
			return {};
		case BINARY:
			return this->bakeBinary();
		case KEYVALUES_STRATA:
			return this->bakeKeyValuesStrata();
	}
	return {};
}

bool CmdSeq::bake(const std::filesystem::path& path) const {
	FileStream writer{path};
	if (!writer) {
		return false;
	}
	writer.seek_out(0).write(this->bake());
	return true;
}
