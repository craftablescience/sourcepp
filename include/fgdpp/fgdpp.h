#pragma once

#include <list>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <sourcepp/math/Vector.h>

class BufferStream;
class BufferStreamReadOnly;

namespace fgdpp {

class FGD {
public:
	struct Entity {
		struct ClassProperty {
			std::string_view name;
			std::string_view arguments;
		};

		struct Field {
			std::string_view name;
			std::string_view valueType;
			bool readonly;
			bool reportable;
			std::string_view displayName;
			std::string_view valueDefault;
			std::string_view description;
		};

		struct FieldChoices {
			struct Choice {
				std::string_view value;
				std::string_view displayName;
			};

			std::string_view name;
			bool readonly;
			bool reportable;
			std::string_view displayName;
			std::string_view valueDefault;
			std::string_view description;
			std::vector<Choice> choices;
		};

		struct FieldFlags {
			struct Flag {
				uint64_t value;
				std::string_view displayName;
				bool enabledByDefault;
				std::string_view description;
			};

			std::string_view name;
			bool readonly;
			bool reportable;
			std::string_view displayName;
			std::string_view description;
			std::vector<Flag> flags;
		};

		struct IO {
			std::string_view name;
			std::string_view valueType;
			std::string_view description;
		};

		std::string_view classType;
		std::vector<ClassProperty> classProperties;

		std::string_view description;

		std::vector<Field> fields;
		std::vector<FieldChoices> fieldsWithChoices;
		std::vector<FieldFlags> fieldsWithFlags;
		std::vector<IO> inputs;
		std::vector<IO> outputs;
	};

	struct AutoVisGroup {
		std::string_view parentName;
		std::string_view name;
		std::vector<std::string_view> entities;
	};

	FGD() = default;

	explicit FGD(const std::string& fgdPath);

	/**
	 * Can be called multiple times in succession to load multiple FGD files.
	 * The FGD file data will be merged with previously loaded data.
	 * @param fgdPath The path to the FGD to load
	 */
	void load(const std::string& fgdPath);

	[[nodiscard]] int getVersion() const;

	[[nodiscard]] sourcepp::math::Vec2i getMapSize() const;

	[[nodiscard]] const std::unordered_map<std::string_view, Entity>& getEntities() const;

	[[nodiscard]] const std::vector<std::string_view>& getMaterialExclusionDirs() const;

	[[nodiscard]] const std::vector<AutoVisGroup>& getAutoVisGroups() const;

	explicit operator bool() const;

private:
	void readEntities(BufferStreamReadOnly& stream, const std::string& path, std::vector<std::string>& seenPaths);

	std::list<std::string> backingData;

	int version = 0;
	sourcepp::math::Vec2i mapSize{};
	std::unordered_map<std::string_view, Entity> entities;
	std::vector<std::string_view> materialExclusionDirs;
	std::vector<AutoVisGroup> autoVisGroups;
};

} // namespace fgdpp
