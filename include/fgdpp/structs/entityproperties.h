#pragma once

#include <string_view>
#include <vector>

namespace fgdpp {

enum class ParseError {
	NO_ERROR = 0,
	TOKENIZATION_ERROR,
	INVALID_DEFINITION,
	INVALID_EQUALS,
	INVALID_OPEN_BRACE,
	INVALID_CLOSE_BRACE,
	INVALID_OPEN_BRACKET,
	INVALID_CLOSE_BRACKET,
	INVALID_OPEN_PARENTHESIS,
	INVALID_CLOSE_PARENTHESIS,
	INVALID_COMMA,
	INVALID_STRING,
	INVALID_PLUS,
	INVALID_LITERAL,
	INVALID_COLUMN,
	INVALID_NUMBER,
	FAILED_TO_OPEN,
	PREMATURE_EOF,
};

struct Range {
	int start;
	int end;
};

struct ParsingError {
	ParseError err;
	int line;
	Range span;
};

#ifdef FGDPP_UNIFIED_FGD
struct TagList {
	std::vector<std::string_view> tags;
};
#endif

struct ClassProperty {
	std::vector<std::string_view> properties;
};

struct ClassProperties {
	std::string_view name;
	std::vector<ClassProperty> classProperties;
};

enum class EntityIOPropertyType {
	t_string = 0,
	t_integer,
	t_float,
	t_bool,
	t_void,
	t_script,
	t_vector,
	t_target_destination,
	t_color255,
	t_custom,
};

struct Choice {
	std::string_view value;
	std::string_view displayName;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

struct Flag {
	int value;
	bool checked;
	std::string_view displayName;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

struct EntityProperties {
	std::string_view propertyName;
	std::string_view type;
	std::string_view displayName;	   // The following 3 are optional and may be empty as a result.
	std::string_view defaultValue;
	std::vector<std::string_view> propertyDescription;
	bool readOnly;
	bool reportable;

#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif

	int choiceCount; // This is a special case if the EntityPropertyType is t_choices
	std::vector<Choice> choices;

	int flagCount; // This is a special case if the EntityPropertyType is t_flags
	std::vector<Flag> flags;
};

enum class IO {
	INPUT = 0,
	OUTPUT,
};

struct InputOutput {
	std::string_view name;
	std::vector<std::string_view> description;
	IO putType;
	std::string_view stringType;
	EntityIOPropertyType type;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

#ifdef FGDPP_UNIFIED_FGD
struct EntityResource {
	std::string_view key;
	std::string_view value;
	TagList tagList;
};
#endif

struct Entity {
	std::string_view type;
	std::vector<ClassProperties> classProperties;
	std::string_view entityName;
	std::vector<std::string_view> entityDescription;
	std::vector<EntityProperties> entityProperties;
	std::vector<InputOutput> inputOutput;
#ifdef FGDPP_UNIFIED_FGD
	std::vector<EntityResource> resources;
#endif
};

struct AutoVisGroupChild {
	std::string_view name;
	std::vector<std::string_view> children;
};

struct AutoVisGroup {
	std::string_view name;
	struct std::vector<AutoVisGroupChild> children;
};

struct FGDFile {
	Range mapSize{0,0};
	std::vector<Entity> entities;
	std::vector<std::string_view> materialExclusions;
	std::vector<std::string_view> includes;
	std::vector<AutoVisGroup> autoVisGroups;
};

} // namespace fgdpp
