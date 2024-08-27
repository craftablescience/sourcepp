#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include <sourcepp/math/Integer.h>

namespace toolpp {

class CmdSeq {
public:
	struct Command {
		bool enabled;

		enum class Special : int32_t {
			NONE = 0,
			CHANGE_DIRECTORY = 256,
			COPY_FILE = 257,
			DELETE_FILE = 258,
			RENAME_FILE = 259,
			COPY_FILE_IF_EXISTS_ALT = 260,
			COPY_FILE_IF_EXISTS = 261,
		} special;
		std::string executable;
		std::string arguments;

		bool ensureFileExists;
		std::string pathToTheoreticallyExistingFile;

		bool useProcessWindow;

		bool waitForKeypress;
	};

	struct Sequence {
		std::string name;
		std::vector<Command> commands;
	};

	explicit CmdSeq(std::string path_);

	[[nodiscard]] float getVersion() const;

	void setVersion(bool isV02);

	[[nodiscard]] std::vector<Sequence>& getSequences();

	[[nodiscard]] const std::vector<Sequence>& getSequences() const;

	[[nodiscard]] std::vector<std::byte> bake() const;

	[[nodiscard]] std::vector<std::byte> bake(bool overrideUsingKeyValues) const;

	bool bake(const std::string& path_);

	bool bake(const std::string& path_, bool overrideUsingKeyValues);

protected:
	void parseBinary(const std::string& path);

	void parseKeyValues(const std::string& path);

	bool usingKeyValues = false;
	float version;
	std::string path;
	std::vector<Sequence> sequences;
};

} // namespace toolpp
