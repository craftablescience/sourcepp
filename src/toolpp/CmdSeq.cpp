#include <toolpp/CmdSeq.h>

#include <FileStream.h>
#include <kvpp/kvpp.h>
#include <sourcepp/String.h>

using namespace kvpp;
using namespace sourcepp;
using namespace toolpp;

CmdSeq::CmdSeq(const std::string& path) {
	bool isBinary;
	{
		FileStream reader{path};
		if (!reader) {
			return;
		}
		if (auto binStr = reader.seek_in(0).read_string(10); binStr == "Worldcraft") {
			isBinary = true;
		} else {
			auto kvStr = reader.seek_in(0).read_string(19);
			string::toLower(kvStr);
			if (kvStr == "\"command sequences\"") {
				isBinary = false;
			} else {
				return;
			}
		}
	}
	if (isBinary) {
		this->parseBinary(path);
	} else {
		this->parseKeyValues(path);
	}
}

void CmdSeq::parseBinary(const std::string& path) {
	FileStream reader{path};
	if (!reader) {
		return;
	}

	auto version = reader.seek_in(31).read<float>();

	auto sequenceCount = reader.read<uint32_t>();
	for (uint32_t s = 0; s < sequenceCount; s++) {
		auto& sequence = this->sequences.emplace_back();
		sequence.name = reader.read_string(128);

		auto commandCount = reader.read<uint32_t>();
		for (uint32_t c = 0; c < commandCount; c++) {
			auto& command = sequence.commands.emplace_back();
			command.enabled = reader.read<int32_t>() & 0xFF;
			command.special = reader.read<Command::Special>();
			command.executable = reader.read_string(260);
			command.arguments = reader.read_string(260);
			reader.skip_in<int32_t>();
			command.ensureFileExists = reader.read<int32_t>();
			command.pathToTheoreticallyExistingFile = reader.read_string(260);
			command.useProcessWindow = reader.read<int32_t>();
			if (version > 0.15f) {
				command.waitForKeypress = reader.read<int32_t>();
			}
		}
	}
}

void CmdSeq::parseKeyValues(const std::string& path) {
	KV1 cmdSeq{fs::readFileText(path)};
	for (const auto& kvSequence : cmdSeq["Command Sequences"].getChildren()) {
		auto& sequence = this->sequences.emplace_back();
		sequence.name = kvSequence.getKey();

		for (const auto& kvCommand : kvSequence.getChildren()) {
			auto& command = sequence.commands.emplace_back();
			string::toBool(kvCommand["enabled"].getValue(), command.enabled);
			string::toInt(kvCommand["special_cmd"].getValue(), reinterpret_cast<std::underlying_type_t<Command::Special>&>(command.special));
			command.executable = kvCommand["run"].getValue();
			command.arguments = kvCommand["params"].getValue();
			string::toBool(kvCommand["ensure_check"].getValue(), command.ensureFileExists);
			command.pathToTheoreticallyExistingFile = kvCommand["ensure_fn"].getValue();
			string::toBool(kvCommand["use_process_wnd"].getValue(), command.useProcessWindow);
			string::toBool(kvCommand["no_wait"].getValue(), command.waitForKeypress);
		}
	}
}

const std::vector<CmdSeq::Sequence>& CmdSeq::getSequences() const {
	return this->sequences;
}
