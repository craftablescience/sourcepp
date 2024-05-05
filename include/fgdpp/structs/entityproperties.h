#pragma once

#include <string_view>
#include <vector>

namespace fgdpp {

enum class ParseError {
	NO_ERROR = 0,
	TOKENIZATION_ERROR,
//    PARSING_ERROR,
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
	std::vector<sourcepp::detail::UtilStringView> tags;
};
#endif

struct ClassProperty {
	std::vector<sourcepp::detail::UtilStringView> properties;
};

struct ClassProperties {
	sourcepp::detail::UtilStringView name;
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
	sourcepp::detail::UtilStringView value;
	sourcepp::detail::UtilStringView displayName;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

struct Flag {
	int value;
	bool checked;
	sourcepp::detail::UtilStringView displayName;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

struct EntityProperties {
	sourcepp::detail::UtilStringView propertyName;
	sourcepp::detail::UtilStringView type;
	sourcepp::detail::UtilStringView displayName;	   // The following 3 are optional and may be empty as a result.
	sourcepp::detail::UtilStringView defaultValue;
	std::vector<sourcepp::detail::UtilStringView> propertyDescription;
	bool readOnly;
	bool reportable;

#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif

    // This is a special case if the EntityPropertyType is t_choices
	std::vector<Choice> choices;

	// This is a special case if the EntityPropertyType is t_flags
	std::vector<Flag> flags;
};

enum class IO {
	INPUT = 0,
	OUTPUT,
};

struct InputOutput {
	sourcepp::detail::UtilStringView name;
	std::vector<sourcepp::detail::UtilStringView> description;
	IO putType;
	sourcepp::detail::UtilStringView stringType;
	EntityIOPropertyType type;
#ifdef FGDPP_UNIFIED_FGD
	TagList tagList;
#endif
};

#ifdef FGDPP_UNIFIED_FGD
struct EntityResource {
	sourcepp::detail::UtilStringView key;
	sourcepp::detail::UtilStringView value;
	TagList tagList;
};
#endif

struct Entity {
	sourcepp::detail::UtilStringView type;
	std::vector<ClassProperties> classProperties;
	sourcepp::detail::UtilStringView entityName;
	std::vector<sourcepp::detail::UtilStringView> entityDescription;
	std::vector<EntityProperties> entityProperties;
	std::vector<InputOutput> inputOutput;
#ifdef FGDPP_UNIFIED_FGD
	std::vector<EntityResource> resources;
#endif
};

struct AutoVisGroupChild {
	sourcepp::detail::UtilStringView name;
	std::vector<sourcepp::detail::UtilStringView> children;
};

struct AutoVisGroup {
	sourcepp::detail::UtilStringView name;
	struct std::vector<AutoVisGroupChild> children;
};

struct FGDFile {
	Range mapSize{0,0};
	std::vector<Entity> entities;
	std::vector<sourcepp::detail::UtilStringView> materialExclusions;
	std::vector<sourcepp::detail::UtilStringView> includes;
	std::vector<AutoVisGroup> autoVisGroups;
};

} // namespace fgdpp
