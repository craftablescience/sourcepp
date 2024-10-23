#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <sourcepp/Math.h>

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
			// This used to be a different thing - Strata changes it to be an alias for 261
			//COPY_FILE_IF_EXISTS_ALT = 260,
			COPY_FILE_IF_EXISTS = 261,
		} special;
		static constexpr auto SPECIAL_COPY_FILE_IF_EXISTS_ALIAS = static_cast<Special>(260);

		std::string executable;
		std::string arguments;

		bool ensureFileExists;
		std::string pathToTheoreticallyExistingFile;

		bool useProcessWindow;

		bool waitForKeypress;

		[[nodiscard]] static std::string getSpecialDisplayNameFor(Special special);

		[[nodiscard]] std::string getExecutableDisplayName() const;
	};

	struct Sequence {
		std::string name;
		std::vector<Command> commands;
	};

	enum class Type {
		INVALID,
		BINARY,
		KEYVALUES_STRATA,
	};

	explicit CmdSeq(std::string path_);

	[[nodiscard]] explicit operator bool() const;

	[[nodiscard]] Type getType() const;

	void setVersion(Type type_);

	[[nodiscard]] float getVersion() const;

	void setVersion(bool isV02);

	[[nodiscard]] std::vector<Sequence>& getSequences();

	[[nodiscard]] const std::vector<Sequence>& getSequences() const;

	[[nodiscard]] std::vector<std::byte> bake() const;

	bool bake(const std::string& path_);

protected:
	void parseBinary(const std::string& path);

	void parseKeyValuesStrata(const std::string& path);

	[[nodiscard]] std::vector<std::byte> bakeBinary() const;

	[[nodiscard]] std::vector<std::byte> bakeKeyValuesStrata() const;

	Type type;
	float version;
	std::string path;
	std::vector<Sequence> sequences;
};

} // namespace toolpp
