#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "structs/EntityProperties.h"

namespace fgdpp {

class FGD {
	std::string rawFGDFile;

	//TOKENIZER
public:
	enum TokenType {
		COMMENT = 0,       // //
		DEFINITION,        // @something
		EQUALS,            // =
		OPEN_BRACE,        // {
		CLOSE_BRACE,       // }
		OPEN_BRACKET,      // [
		CLOSE_BRACKET,     // ]
		OPEN_PARENTHESIS,  // (
		CLOSE_PARENTHESIS, // )
		COMMA,             // ,
		STRING,            // "something"
		PLUS,              // +
		LITERAL,           // anything that isn't any of the other tokens.
		COLUMN,            // :
		NUMBER,            // numbers -200000 ... 0 ... 2000000
	};

	struct Token {
		TokenType type;
		Range range;
		std::string_view string;
		int line;
		ParseError associatedError;
	};

	std::vector<Token> tokenList;

public:
	FGD(std::string_view path, bool parseIncludes);

private:
	bool TokenizeFile();

	//PARSER.
public:
	FGDFile FGDFileContents;
	ParsingError parseError{ParseError::NO_ERROR, 0, {0, 0}};

	bool ParseFile();

#ifdef FGDPP_UNIFIED_FGD
	bool TagListDelimiter(std::vector<Token>::const_iterator& iter, TagList& tagList);
#endif
};

} // namespace fgdpp
