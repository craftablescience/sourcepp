#pragma once

#include <initializer_list>
#include <list>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/Math.h>

namespace toolpp {

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
		std::string_view docsURL;

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

	explicit FGD(const std::filesystem::path& fgdPath);

	/**
	 * Can be called multiple times in succession to load multiple FGD files.
	 * The FGD file data will be merged with previously loaded data.
	 * @param fgdPath The path to the FGD to load
	 */
	void load(const std::filesystem::path& fgdPath);

	[[nodiscard]] int getVersion() const;

	[[nodiscard]] sourcepp::math::Vec2i getMapSize() const;

	[[nodiscard]] const std::unordered_map<std::string_view, Entity>& getEntities() const;

	[[nodiscard]] const std::vector<std::string_view>& getMaterialExclusionDirs() const;

	[[nodiscard]] const std::vector<AutoVisGroup>& getAutoVisGroups() const;

protected:
	void readEntities(BufferStreamReadOnly& stream, const std::filesystem::path& path, std::vector<std::filesystem::path>& seenPaths);

	std::list<std::string> backingData;

	int version = 0;
	sourcepp::math::Vec2i mapSize{};
	std::unordered_map<std::string_view, Entity> entities;
	std::vector<std::string_view> materialExclusionDirs;
	std::vector<AutoVisGroup> autoVisGroups;
};

class FGDWriter {
public:
	class AutoVisGroupWriter {
	public:
		explicit AutoVisGroupWriter(FGDWriter& parent_);

		AutoVisGroupWriter& visGroup(std::string_view name, std::initializer_list<std::string_view> entities);

		AutoVisGroupWriter& visGroup(std::string_view name, std::span<const std::string_view> entities);

		FGDWriter& endAutoVisGroup() const; // NOLINT(*-use-nodiscard)

	private:
		FGDWriter& parent;
	};

	class EntityWriter {
	public:
		class KeyValueChoicesWriter {
		public:
			explicit KeyValueChoicesWriter(EntityWriter& parent_);

			KeyValueChoicesWriter& choice(std::string_view value, std::string_view displayName);

			EntityWriter& endKeyValueChoices() const; // NOLINT(*-use-nodiscard)

		private:
			EntityWriter& parent;
		};

		class KeyValueFlagsWriter {
		public:
			explicit KeyValueFlagsWriter(EntityWriter& parent_);

			KeyValueFlagsWriter& flag(uint64_t value, std::string_view displayName, bool enabledByDefault, std::string_view description = "");

			EntityWriter& endKeyValueFlags() const; // NOLINT(*-use-nodiscard)

		private:
			EntityWriter& parent;
		};

		explicit EntityWriter(FGDWriter& parent_);

		EntityWriter& keyValue(std::string_view name, std::string_view valueType, std::string_view displayName = "", std::string_view valueDefault = "", std::string_view description = "", bool readOnly = false, bool report = false);

		KeyValueChoicesWriter beginKeyValueChoices(std::string_view name, std::string_view displayName = "", std::string_view valueDefault = "", std::string_view description = "", bool readOnly = false, bool report = false);

		KeyValueFlagsWriter beginKeyValueFlags(std::string_view name, std::string_view displayName = "", std::string_view description = "", bool readOnly = false, bool report = false);

		EntityWriter& input(std::string_view name, std::string_view valueType, std::string_view description = "");

		EntityWriter& output(std::string_view name, std::string_view valueType, std::string_view description = "");

		FGDWriter& endEntity() const; // NOLINT(*-use-nodiscard)

	private:
		FGDWriter& parent;
	};

	[[nodiscard]] static FGDWriter begin();

	FGDWriter& include(const std::filesystem::path& fgdPath);

	FGDWriter& version(int version);

	FGDWriter& mapSize(sourcepp::math::Vec2i mapSize);

	FGDWriter& materialExclusionDirs(std::initializer_list<std::string_view> dirs);

	FGDWriter& materialExclusionDirs(std::span<const std::string_view> dirs);

	AutoVisGroupWriter beginAutoVisGroup(std::string_view parentName);

	EntityWriter beginEntity(std::string_view classType, std::initializer_list<std::string_view> classProperties, std::string_view name, std::string_view description = "", std::string_view docsURL = "");

	EntityWriter beginEntity(std::string_view classType, std::span<const std::string_view> classProperties, std::string_view name, std::string_view description = "", std::string_view docsURL = "");

	[[nodiscard]] std::string bake() const;

	bool bake(const std::filesystem::path& fgdPath) const; // NOLINT(*-use-nodiscard)

protected:
	FGDWriter();

	std::string backingData;
	BufferStream writer;
};

} // namespace fgdpp
