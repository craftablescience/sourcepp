#include <toolpp/FGD.h>

#include <algorithm>
#include <filesystem>
#include <initializer_list>

#include <BufferStream.h>

#include <sourcepp/parser/Text.h>
#include <sourcepp/FS.h>
#include <sourcepp/String.h>

using namespace sourcepp;
using namespace toolpp;

namespace {

constexpr const char* INVALID_SYNTAX_MSG = "Invalid syntax found in FGD!";
constexpr const char* INVALID_CLASS_MSG = "Invalid class found in FGD!";

[[nodiscard]] bool tryToEatSeparator(BufferStream& stream, char sep) {
	parser::text::eatWhitespaceAndSingleLineComments(stream);
	return parser::text::tryToEatChar(stream, sep);
}

// FGD strings are weird - they can be split across several lines, only accept \n escapes,
// and don't need to be terminated by a double quote. Really gross...
[[nodiscard]] std::string_view readFGDString(BufferStreamReadOnly& stream, BufferStream& backing) {
	parser::text::eatWhitespaceAndSingleLineComments(stream);

	static constexpr std::string_view END = "\"\n";

	auto startSpan = backing.tell();
	while (true) {
		char c = stream.read<char>();
		if (c != '\"') {
			stream.seek(-1, std::ios::cur);
			auto out = parser::text::readUnquotedStringToBuffer(stream, backing, ":", parser::text::NO_ESCAPE_SEQUENCES);
			if (stream.seek(-1, std::ios::cur).peek<char>() != ':') {
				stream.skip();
				parser::text::eatWhitespaceAndSingleLineComments(stream);
			}
			return out;
		}

		for (c = stream.read<char>(); END.find(c) == std::string_view::npos; c = stream.read<char>()) {
			if (c == '\\') {
				auto n = stream.read<char>();
				if (n == 'n') {
					backing << '\n';
				} else if (END.find(n) != std::string_view::npos) {
					break;
				} else {
					backing << c << n;
				}
			} else {
				backing << c;
			}
		}

		if (!::tryToEatSeparator(stream, '+')) {
			break;
		}
		// We need to make sure the next line is actually a string, because sometimes + will lead to nothing. Lovely
		parser::text::eatWhitespaceAndSingleLineComments(stream);
		if (stream.peek<char>() != '\"') {
			break;
		}
	}

	if (stream.seek(-1, std::ios::cur).peek<char>() != ':') {
		stream.skip();
		parser::text::eatWhitespaceAndSingleLineComments(stream);
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, backing.tell() - 1 - startSpan};
}

void readVersion(BufferStreamReadOnly& stream, BufferStream& backing, int& version) {
	if (stream.seek(-1, std::ios::cur).peek<char>() != '(') {
		parser::text::eatWhitespace(stream);
		if (stream.peek<char>() != '(') {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
	}
	std::string versionString{parser::text::readStringToBuffer(stream, backing, "(", ")", parser::text::NO_ESCAPE_SEQUENCES)};
	string::trim(versionString);
	string::toInt(versionString, version);
}

void readMapSize(BufferStreamReadOnly& stream, BufferStream& backing, math::Vec2i& mapSize) {
	if (stream.seek(-1, std::ios::cur).peek<char>() != '(') {
		parser::text::eatWhitespace(stream);
		if (stream.peek<char>() != '(') {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
	}
	auto mapSizeString = parser::text::readStringToBuffer(stream, backing, "(", ")", parser::text::NO_ESCAPE_SEQUENCES);
	auto mapSizes = string::split(mapSizeString, ',');
	if (mapSizes.size() != 2) {
		throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
	}

	string::trim(mapSizes[0]);
	string::trim(mapSizes[1]);
	string::toInt(mapSizes[0], mapSize[0]);
	string::toInt(mapSizes[1], mapSize[1]);
}

void readMaterialExclusionDirs(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<std::string_view>& materialExclusionDirs) {
	if (!::tryToEatSeparator(stream, '[')) {
		throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
	}
	while (!::tryToEatSeparator(stream, ']')) {
		materialExclusionDirs.push_back(::readFGDString(stream, backing));
	}
	stream.skip();
}

void readAutoVisGroups(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<FGD::AutoVisGroup>& autoVisGroups) {
	if (!::tryToEatSeparator(stream, '=')) {
		throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
	}
	auto parentName = ::readFGDString(stream, backing);
	if (!::tryToEatSeparator(stream, '[')) {
		throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
	}
	while (!::tryToEatSeparator(stream, ']')) {
		auto& autoVisGroup = autoVisGroups.emplace_back();
		autoVisGroup.parentName = parentName;
		autoVisGroup.name = ::readFGDString(stream, backing);
		if (!::tryToEatSeparator(stream, '[')) {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
		while (!::tryToEatSeparator(stream, ']')) {
			autoVisGroup.entities.push_back(::readFGDString(stream, backing));
		}
		stream.skip();
	}
	stream.skip();
}

void readClassProperties(BufferStreamReadOnly& stream, BufferStream& backing, FGD::Entity& entity) {
	parser::text::eatWhitespaceAndSingleLineComments(stream);

	while (stream.peek<char>() != '=') {
		FGD::Entity::ClassProperty classProperty;
		classProperty.name = parser::text::readUnquotedStringToBuffer(stream, backing, "(", parser::text::NO_ESCAPE_SEQUENCES);
		classProperty.arguments = "";

		if (stream.seek(-1, std::ios::cur).peek<char>() != '(') {
			parser::text::eatWhitespace(stream);
			if (stream.peek<char>() != '(') {
				entity.classProperties.push_back(classProperty);
				continue;
			}
		}
		classProperty.arguments = parser::text::readStringToBuffer(stream, backing, "(", ")", {{'n', '\n'}});
		entity.classProperties.push_back(classProperty);

		parser::text::eatWhitespaceAndSingleLineComments(stream);
	}

	stream.skip();
	parser::text::eatWhitespaceAndSingleLineComments(stream);
}

void readEntityIO(BufferStreamReadOnly& stream, BufferStream& backing, FGD::Entity& entity, bool input) {
	auto& io = input ? entity.inputs.emplace_back() : entity.outputs.emplace_back();
	io.name = parser::text::readUnquotedStringToBuffer(stream, backing, "(", parser::text::NO_ESCAPE_SEQUENCES);
	if (stream.seek(-1, std::ios::cur).peek<char>() != '(') {
		parser::text::eatWhitespace(stream);
		if (stream.peek<char>() != '(') {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
	}
	io.valueType = parser::text::readStringToBuffer(stream, backing, "(", ")", parser::text::NO_ESCAPE_SEQUENCES);

	if (!::tryToEatSeparator(stream, ':')) {
		io.description = "";
		return;
	}
	io.description = ::readFGDString(stream, backing);

	parser::text::eatWhitespaceAndSingleLineComments(stream);
}

void readEntityFieldModifiers(BufferStreamReadOnly& stream, BufferStream& backing, bool& readonly, bool& reportable) {
	readonly = false;
	reportable = false;

	parser::text::eatWhitespace(stream);
	if (stream.peek<char>() == 'r') {
		if (auto modifier = parser::text::readUnquotedStringToBuffer(stream, backing); modifier == "readonly") {
			readonly = true;
		} else if (modifier == "report") {
			reportable = true;
			return;
		} else {
			stream.seek(-static_cast<int64_t>(modifier.length()), std::ios::cur);
			return;
		}
	}
	parser::text::eatWhitespace(stream);
	if (stream.peek<char>() == 'r') {
		if (auto modifier = parser::text::readUnquotedStringToBuffer(stream, backing); modifier == "report") {
			reportable = true;
		}  else {
			stream.seek(-static_cast<int64_t>(modifier.length()), std::ios::cur);
			return;
		}
	}
}

void readEntityKeyValue(BufferStreamReadOnly& stream, BufferStream& backing, FGD::Entity& entity) {
	// Key and value type (looks like "key(valueType)", or "input key(valueType)" for i/o)
	auto name = parser::text::readUnquotedStringToBuffer(stream, backing, "(", parser::text::NO_ESCAPE_SEQUENCES);
	parser::text::eatWhitespace(stream);
	if (string::iequals(name, "input")) {
		::readEntityIO(stream, backing, entity, true);
		return;
	} else if (string::iequals(name, "output")) {
		::readEntityIO(stream, backing, entity, false);
		return;
	}
	auto valueType = parser::text::readUnquotedStringToBuffer(stream, backing, ")", parser::text::NO_ESCAPE_SEQUENCES);
	// If there is a space after the value type, we need to get rid of the parenthesis here
	parser::text::eatWhitespace(stream);
	if (stream.peek<char>() == ')') {
		stream.skip();
	}

	if (string::iequals(valueType, "choices")) {
		auto& field = entity.fieldsWithChoices.emplace_back();
		field.name = name;
		::readEntityFieldModifiers(stream, backing, field.readonly, field.reportable);

		if (::tryToEatSeparator(stream, ':')) {
			field.displayName = ::readFGDString(stream, backing);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}

		if (::tryToEatSeparator(stream, ':')) {
			field.valueDefault = ::readFGDString(stream, backing);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}

		if (::tryToEatSeparator(stream, ':')) {
			field.description = ::readFGDString(stream, backing);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}

		if (!::tryToEatSeparator(stream, '=') || !::tryToEatSeparator(stream, '[')) {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
		while (stream.peek<char>() != ']') {
			auto& choice = field.choices.emplace_back();
			choice.value = ::readFGDString(stream, backing);

			if (::tryToEatSeparator(stream, ':')) {
				choice.displayName = ::readFGDString(stream, backing);
			} else {
				choice.displayName = "";
			}
		}
		stream.skip();
	} else if (string::iequals(valueType, "flags")) {
		auto& field = entity.fieldsWithFlags.emplace_back();
		field.name = name;
		::readEntityFieldModifiers(stream, backing, field.readonly, field.reportable);

		if (::tryToEatSeparator(stream, ':')) {
			field.displayName = ::readFGDString(stream, backing);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}

		if (::tryToEatSeparator(stream, ':')) {
			field.description = ::readFGDString(stream, backing);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}

		if (!::tryToEatSeparator(stream, '=') || !::tryToEatSeparator(stream, '[')) {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}
		parser::text::eatWhitespaceAndSingleLineComments(stream);

		while (stream.peek<char>() != ']') {
			auto& flag = field.flags.emplace_back();
			auto valueString = ::readFGDString(stream, backing);
			if (string::toInt(valueString, flag.value).ec != std::errc{}) {
				flag.value = 0;
			}

			if (!::tryToEatSeparator(stream, ':')) {
				continue;
			}
			flag.displayName = ::readFGDString(stream, backing);

			if (!::tryToEatSeparator(stream, ':')) {
				continue;
			}
			auto enabledByDefaultString = ::readFGDString(stream, backing);
			int enabledByDefault = 0;
			if (string::toInt(enabledByDefaultString, enabledByDefault).ec != std::errc{}) {
				flag.enabledByDefault = false;
			} else {
				flag.enabledByDefault = static_cast<bool>(enabledByDefault);
			}

			if (!::tryToEatSeparator(stream, ':')) {
				continue;
			}
			flag.description = ::readFGDString(stream, backing);
		}
		stream.skip();
	} else {
		auto& field = entity.fields.emplace_back();
		field.name = name;
		field.valueType = valueType;
		::readEntityFieldModifiers(stream, backing, field.readonly, field.reportable);
		field.displayName = "";
		field.valueDefault = "";
		field.description = "";

		if (!::tryToEatSeparator(stream, ':')) {
			return;
		}
		field.displayName = ::readFGDString(stream, backing);

		if (!::tryToEatSeparator(stream, ':')) {
			return;
		}
		field.valueDefault = ::readFGDString(stream, backing);

		if (!::tryToEatSeparator(stream, ':')) {
			return;
		}
		field.description = ::readFGDString(stream, backing);

		parser::text::eatWhitespaceAndSingleLineComments(stream);
	}
}

void overwriteEntity(FGD::Entity& oldEntity, FGD::Entity& newEntity) {
	oldEntity.classProperties = newEntity.classProperties;
	if (!newEntity.description.empty()) {
		oldEntity.description = newEntity.description;
	}
	for (const auto& field : newEntity.fields) {
		if (auto it = std::find_if(oldEntity.fields.begin(), oldEntity.fields.end(), [&field](const auto& oldField) {
				return oldField.name == field.name;
		}); it != oldEntity.fields.end()) {
			it->valueType = field.valueType;
			it->readonly = field.readonly;
			it->reportable = field.reportable;
			if (!field.displayName.empty()) {
				it->displayName = field.displayName;
			}
			if (!field.valueDefault.empty()) {
				it->valueDefault = field.valueDefault;
			}
			if (!field.description.empty()) {
				it->description = field.description;
			}
		} else {
			oldEntity.fields.push_back(field);
		}
	}
	for (const auto& field : newEntity.fieldsWithChoices) {
		if (auto it = std::find_if(oldEntity.fieldsWithChoices.begin(), oldEntity.fieldsWithChoices.end(), [&field](const auto& oldField) {
				return oldField.name == field.name;
		}); it != oldEntity.fieldsWithChoices.end()) {
			it->readonly = field.readonly;
			it->reportable = field.reportable;
			if (!field.displayName.empty()) {
				it->displayName = field.displayName;
			}
			if (!field.valueDefault.empty()) {
				it->valueDefault = field.valueDefault;
			}
			if (!field.description.empty()) {
				it->description = field.description;
			}
			it->choices = field.choices;
		} else {
			oldEntity.fieldsWithChoices.push_back(field);
		}
	}
	for (const auto& field : newEntity.fieldsWithFlags) {
		if (auto it = std::find_if(oldEntity.fieldsWithFlags.begin(), oldEntity.fieldsWithFlags.end(), [&field](const auto& oldField) {
				return oldField.name == field.name;
		}); it != oldEntity.fieldsWithFlags.end()) {
			it->readonly = field.readonly;
			it->reportable = field.reportable;
			it->flags = field.flags;
		} else {
			oldEntity.fieldsWithFlags.push_back(field);
		}
	}
	for (const auto& input : newEntity.inputs) {
		if (auto it = std::find_if(oldEntity.inputs.begin(), oldEntity.inputs.end(), [&input](const auto& oldInput) {
				return oldInput.name == input.name;
		}); it != oldEntity.inputs.end()) {
			it->valueType = input.valueType;
			if (!input.description.empty()) {
				it->description = input.description;
			}
		} else {
			oldEntity.inputs.push_back(input);
		}
	}
	for (const auto& output : newEntity.outputs) {
		if (auto it = std::find_if(oldEntity.outputs.begin(), oldEntity.outputs.end(), [&output](const auto& oldOutput) {
				return oldOutput.name == output.name;
		}); it != oldEntity.outputs.end()) {
			it->valueType = output.valueType;
			if (!output.description.empty()) {
				it->description = output.description;
			}
		} else {
			oldEntity.outputs.push_back(output);
		}
	}
}

void readEntity(BufferStreamReadOnly& stream, BufferStream& backing, std::string_view classType, std::unordered_map<std::string_view, FGD::Entity>& entities, bool extension) {
	FGD::Entity entity{};
	entity.classType = classType;

	// There are optionally a list of class properties after the class
	if (!::tryToEatSeparator(stream, '=')) {
		::readClassProperties(stream, backing, entity);
	}

	// Entity name
	parser::text::eatWhitespaceAndSingleLineComments(stream);
	auto name = ::readFGDString(stream, backing);

	// If a colon is here, the entity has a description
	if (::tryToEatSeparator(stream, ':')) {
		entity.description = ::readFGDString(stream, backing);
	} else {
		entity.description = "";
	}

	// Parse entity keyvalues
	if (!::tryToEatSeparator(stream, '[')) {
		throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
	}
	while (!::tryToEatSeparator(stream, ']')) {
		::readEntityKeyValue(stream, backing, entity);
	}
	stream.skip();

	if (extension && entities.contains(name)) {
		// Overwrite/add parts of the entity description
		::overwriteEntity(entities[name], entity);
	} else {
		// Replace entity description entirely
		entities[name] = entity;
	}
}

void writeOptionalKeyValueStrings(BufferStream& writer, std::initializer_list<std::string_view> strings) {
	static constexpr auto writeOptionalString = [](BufferStream& stream, const std::string& str) {
		if (!str.empty()) {
			stream
				.write(" : \"", 4)
				.write(str, false)
				.write('\"');
		} else {
			stream.write(" :", 2);
		}
	};
	for (auto revString = std::rbegin(strings); revString != std::rend(strings); ++revString) {
		if (revString->empty()) {
			continue;
		}
		for (auto string = std::begin(strings); string != revString.base(); ++string) {
			writeOptionalString(writer, std::string{*string});
		}
		return;
	}
}

} // namespace

FGD::FGD(const std::string& fgdPath) {
	this->load(fgdPath);
}

void FGD::load(const std::string& fgdPath) {
	auto fgdData = fs::readFileText(fgdPath);
	if (fgdData.empty()) {
		return;
	}
	BufferStreamReadOnly stream{fgdData};

	try {
		std::vector<std::string> seenPaths{fgdPath};
		string::normalizeSlashes(seenPaths.front());
		this->readEntities(stream, fgdPath, seenPaths);
	} catch (const std::overflow_error&) {}
}

int FGD::getVersion() const {
	return this->version;
}

math::Vec2i FGD::getMapSize() const {
	return this->mapSize;
}

const std::unordered_map<std::string_view, FGD::Entity>& FGD::getEntities() const {
	return this->entities;
}

const std::vector<std::string_view>& FGD::getMaterialExclusionDirs() const {
	return this->materialExclusionDirs;
}

const std::vector<FGD::AutoVisGroup>& FGD::getAutoVisGroups() const {
	return this->autoVisGroups;
}

// NOLINTNEXTLINE(*-no-recursion)
void FGD::readEntities(BufferStreamReadOnly& stream, const std::string& path, std::vector<std::string>& seenPaths) {
	auto& backingString = this->backingData.emplace_back();
	// Multiply by 2 to ensure buffer will have enough space (very generous)
	backingString.resize(stream.size() * 2);
	BufferStream backing{backingString, false};

	while (true) {
		parser::text::eatWhitespaceAndSingleLineComments(stream);

		// All entity definitions start with an '@' followed by the class type
		if (stream.read<char>() != '@') {
			throw parser::text::syntax_error{INVALID_SYNTAX_MSG};
		}

		auto classType = parser::text::readUnquotedStringToBuffer(stream, backing, "(", parser::text::NO_ESCAPE_SEQUENCES);
		if (string::iequals(classType, "include")) {
			parser::text::eatWhitespace(stream);
			// Assume the include path is relative to the current file being processed
			auto fgdPath = (std::filesystem::path{path}.parent_path() / parser::text::readStringToBuffer(stream, backing)).string();
			string::normalizeSlashes(fgdPath);
			if (std::find(seenPaths.begin(), seenPaths.end(), fgdPath) != seenPaths.end()) {
				continue;
			}
			seenPaths.push_back(fgdPath);

			auto fgdData = fs::readFileText(fgdPath);
			if (fgdData.empty()) {
				continue;
			}

			BufferStreamReadOnly newStream{fgdData};
			try {
				this->readEntities(newStream, fgdPath, seenPaths);
			} catch (const std::overflow_error&) {}
		} else if (string::iequals(classType, "version")) {
			::readVersion(stream, backing, this->version);
		} else if (string::iequals(classType, "mapsize")) {
			::readMapSize(stream, backing, this->mapSize);
		} else if (string::iequals(classType, "MaterialExclusion")) {
			::readMaterialExclusionDirs(stream, backing, this->materialExclusionDirs);
		} else if (string::iequals(classType, "AutoVisGroup")) {
			::readAutoVisGroups(stream, backing, this->autoVisGroups);
		} else if (string::iequals(classType, "BaseClass") ||
				   string::iequals(classType, "PointClass") ||
				   string::iequals(classType, "NPCClass") ||
				   string::iequals(classType, "SolidClass") ||
				   string::iequals(classType, "KeyFrameClass") ||
				   string::iequals(classType, "MoveClass") ||
				   string::iequals(classType, "FilterClass")) {
			::readEntity(stream, backing, classType, this->entities, false);
		} else if (string::iequals(classType, "ExtendClass")) {
			::readEntity(stream, backing, classType, this->entities, true);
		} else {
			throw parser::text::syntax_error{INVALID_CLASS_MSG};
		}
	}
}

FGDWriter::AutoVisGroupWriter::AutoVisGroupWriter(FGDWriter& parent_)
		: parent(parent_) {}

FGDWriter::EntityWriter::EntityWriter(FGDWriter& parent_)
		: parent(parent_) {}

FGDWriter::EntityWriter::KeyValueChoicesWriter::KeyValueChoicesWriter(FGDWriter::EntityWriter& parent_)
		: parent(parent_) {}

FGDWriter::EntityWriter::KeyValueFlagsWriter::KeyValueFlagsWriter(FGDWriter::EntityWriter& parent_)
		: parent(parent_) {}

FGDWriter::FGDWriter()
		: writer(backingData) {}

FGDWriter FGDWriter::begin() {
	return {};
}

FGDWriter& FGDWriter::include(const std::string& fgdPath) {
	this->writer
		.write("@include \"", 10)
		.write(fgdPath, false)
		.write("\"\n\n", 3);
	return *this;
}

FGDWriter& FGDWriter::version(int version) {
	this->writer
		.write("@version(", 9)
		.write(std::to_string(version), false)
		.write(")\n\n", 3);
	return *this;
}

FGDWriter& FGDWriter::mapSize(math::Vec2i mapSize) {
	this->writer
	    .write("@mapsize(", 9)
	    .write(std::to_string(mapSize[0]), false)
	    .write(", ", 2)
	    .write(std::to_string(mapSize[1]), false)
	    .write(")\n\n", 3);
	return *this;
}

FGDWriter& FGDWriter::materialExclusionDirs(const std::vector<std::string>& dirs) {
	this->writer.write("@MaterialExclusion\n[\n", 21);
	for (const auto& dir : dirs) {
		this->writer << '\t' << '\"';
		this->writer.write(dir, false);
		this->writer << '\"' << '\n';
	}
	this->writer.write("]\n\n", 3);
	return *this;
}

FGDWriter::AutoVisGroupWriter FGDWriter::beginAutoVisGroup(const std::string& parentName) {
	this->writer
		.write("@AutoVisGroup = \"", 17)
		.write(parentName, false)
		.write("\"\n[\n", 4);
	return AutoVisGroupWriter{*this};
}

FGDWriter::AutoVisGroupWriter& FGDWriter::AutoVisGroupWriter::visGroup(const std::string& name, const std::vector<std::string>& entities) {
	this->parent.writer
	    .write("\t\"", 2)
	    .write(name, false)
	    .write("\"\n\t[\n", 5);
	for (const auto& entity : entities) {
		this->parent.writer
			.write("\t\t\"", 3)
			.write(entity, false)
			.write("\"\n", 2);
	}
	this->parent.writer.write("\t]\n", 3);
	return *this;
}

FGDWriter& FGDWriter::AutoVisGroupWriter::endAutoVisGroup() {
	this->parent.writer.write("]\n\n", 3);
	return this->parent;
}

FGDWriter::EntityWriter FGDWriter::beginEntity(const std::string& classType, const std::vector<std::string>& classProperties, const std::string& name, const std::string& description) {
	this->writer
		.write('@')
		.write(classType, false);
	if (classProperties.empty()) {
		this->writer << ' ';
	} else {
		this->writer << '\n';
		for (const auto& classProperty : classProperties) {
			this->writer
			    .write('\t')
			    .write(classProperty, false)
			    .write('\n');
		}
	}
	this->writer
		.write("= ", 2)
		.write(name, false)
		.write(" :", 2);
	// Put the description on the same line if it's short
	if (description.size() < 32) {
		this->writer
			.write(" \"", 2)
			.write(description, false);
	} else {
		this->writer
			.write("\n\t\"", 3)
			.write(description, false);
	}
	this->writer.write("\"\n[\n", 4);
	return EntityWriter{*this};
}

FGDWriter::EntityWriter& FGDWriter::EntityWriter::keyValue(const std::string& name, const std::string& valueType, const std::string& displayName, const std::string& valueDefault, const std::string& description, bool readOnly, bool report) {
	this->parent.writer
		.write('\t')
		.write(name, false)
		.write('(')
		.write(valueType, false)
		.write(')');
	if (readOnly) {
		this->parent.writer.write(" readonly", 9);
	}
	if (report) {
		this->parent.writer.write(" report", 7);
	}
	::writeOptionalKeyValueStrings(this->parent.writer, {displayName, valueDefault, description});
	this->parent.writer << '\n';
	return *this;
}

FGDWriter::EntityWriter::KeyValueChoicesWriter FGDWriter::EntityWriter::beginKeyValueChoices(const std::string& name, const std::string& displayName, const std::string& valueDefault, const std::string& description, bool readOnly, bool report) {
	this->parent.writer
	    .write('\t')
	    .write(name, false)
	    .write("(choices)", 9);
	if (readOnly) {
		this->parent.writer.write(" readonly", 9);
	}
	if (report) {
		this->parent.writer.write(" report", 7);
	}
	::writeOptionalKeyValueStrings(this->parent.writer, {displayName, valueDefault, description});
	this->parent.writer.write(" =\n\t[\n", 6);
	return KeyValueChoicesWriter{*this};
}

FGDWriter::EntityWriter::KeyValueChoicesWriter& FGDWriter::EntityWriter::KeyValueChoicesWriter::choice(const std::string& value, const std::string& displayName) {
	this->parent.parent.writer
		.write("\t\t\"", 3)
		.write(value, false)
		.write("\" : \"", 5)
		.write(displayName, false)
		.write("\"\n", 2);
	return *this;
}

FGDWriter::EntityWriter& FGDWriter::EntityWriter::KeyValueChoicesWriter::endKeyValueChoices() {
	this->parent.parent.writer.write("\t]\n", 3);
	return this->parent;
}

FGDWriter::EntityWriter::KeyValueFlagsWriter FGDWriter::EntityWriter::beginKeyValueFlags(const std::string& name, const std::string& displayName, const std::string& description, bool readOnly, bool report) {
	this->parent.writer
	    .write('\t')
	    .write(name, false)
	    .write("(flags)", 7);
	if (readOnly) {
		this->parent.writer.write(" readonly", 9);
	}
	if (report) {
		this->parent.writer.write(" report", 7);
	}
	::writeOptionalKeyValueStrings(this->parent.writer, {displayName, description});
	this->parent.writer.write(" =\n\t[\n", 6);
	return KeyValueFlagsWriter{*this};
}

FGDWriter::EntityWriter::KeyValueFlagsWriter& FGDWriter::EntityWriter::KeyValueFlagsWriter::flag(uint64_t value, const std::string& displayName, bool enabledByDefault, const std::string& description) {
	this->parent.parent.writer
		.write("\t\t", 2)
	    .write(std::to_string(value), false)
	    .write(" : \"", 4)
	    .write(displayName, false)
	    .write("\" : ", 4)
	    .write(std::to_string(enabledByDefault), false);
	if (!description.empty()) {
		this->parent.parent.writer
		    .write(" : \"", 4)
		    .write(description, false)
		    .write('\"');
	}
	this->parent.parent.writer.write('\n');
	return *this;
}

FGDWriter::EntityWriter& FGDWriter::EntityWriter::KeyValueFlagsWriter::endKeyValueFlags() {
	this->parent.parent.writer.write("\t]\n", 3);
	return this->parent;
}

FGDWriter::EntityWriter& FGDWriter::EntityWriter::input(const std::string& name, const std::string& valueType, const std::string& description) {
	this->parent.writer
	    .write('\t')
		.write("input ", 6)
	    .write(name, false)
	    .write('(')
	    .write(valueType, false)
	    .write(')');
	if (!description.empty()) {
		this->parent.writer
		    .write(" : \"", 4)
		    .write(description, false)
		    .write('\"');
	}
	this->parent.writer << '\n';
	return *this;
}

FGDWriter::EntityWriter& FGDWriter::EntityWriter::output(const std::string& name, const std::string& valueType, const std::string& description) {
	this->parent.writer
	    .write('\t')
	    .write("output ", 7)
	    .write(name, false)
	    .write('(')
	    .write(valueType, false)
	    .write(')');
	if (!description.empty()) {
		this->parent.writer
		    .write(" : \"", 4)
		    .write(description, false)
		    .write('\"');
	}
	this->parent.writer << '\n';
	return *this;
}

FGDWriter& FGDWriter::EntityWriter::endEntity() {
	this->parent.writer.write("]\n\n", 3);
	return this->parent;
}

std::string FGDWriter::bake() {
	this->backingData.resize(this->writer.tell());
	if (this->backingData.ends_with("\n\n")) {
		this->backingData.pop_back();
	}
	return this->backingData;
}

void FGDWriter::bake(const std::string& fgdPath) {
	fs::writeFileText(fgdPath, this->bake());
}
