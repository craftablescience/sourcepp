#include <fgdpp/fgdpp.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <regex>
#include <iostream>

using namespace fgdpp;
using namespace sourcepp::detail;

constexpr int FGDPP_MAX_STR_CHUNK_LENGTH = 1024;

constexpr const char singleTokens[11] = R"({}[](),:=+)";
constexpr FGD::TokenType valueTokens[10] = {FGD::OPEN_BRACE, FGD::CLOSE_BRACE, FGD::OPEN_BRACKET, FGD::CLOSE_BRACKET, FGD::OPEN_PARENTHESIS, FGD::CLOSE_PARENTHESIS, FGD::COMMA, FGD::COLUMN, FGD::EQUALS, FGD::PLUS};
constexpr enum ParseError tokenErrors[10] = {ParseError::INVALID_OPEN_BRACE, ParseError::INVALID_CLOSE_BRACE, ParseError::INVALID_OPEN_BRACKET, ParseError::INVALID_CLOSE_BRACKET, ParseError::INVALID_OPEN_PARENTHESIS, ParseError::INVALID_CLOSE_PARENTHESIS, ParseError::INVALID_COMMA, ParseError::INVALID_COLUMN, ParseError::INVALID_EQUALS, ParseError::INVALID_PLUS};

std::string_view typeStrings[9] = {"string", "integer", "float", "bool", "void", "script", "vector", "target_destination", "color255"};
EntityIOPropertyType typeList[9] = {EntityIOPropertyType::t_string, EntityIOPropertyType::t_integer, EntityIOPropertyType::t_float, EntityIOPropertyType::t_bool, EntityIOPropertyType::t_void, EntityIOPropertyType::t_script, EntityIOPropertyType::t_vector, EntityIOPropertyType::t_target_destination, EntityIOPropertyType::t_color255};

namespace {

inline bool ichar_equals(char a, char b) {
	return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
}

inline bool iequals(std::string_view lhs, std::string_view rhs) {
	return std::ranges::equal(lhs, rhs, ichar_equals);
}

} // namespace

bool FGD::TokenizeFile() {
    if (this->rawFGDFile.empty())
        return false;

    this->tokenList.reserve(rawFGDFile.length() / 2);

    int pos = 1, ln = 1, i = 0;

    for (auto iterator = this->rawFGDFile.cbegin(); iterator != this->rawFGDFile.cend(); iterator++, i++, pos++) {
        char c = *iterator;

        if (c == '\t')
            continue;

        if (c == '\r')
            continue;

        if (c == '\n') {
            ln++;
            pos = 1;
            continue;
        }

        if (c == '\"') {
            int currentLine = ln;
            int currentLength = i;
            int currentPos = pos;
            auto currentIteration = iterator;

            c = '\t'; // We can get away with this to trick the while loop :)
            while (c != '\"') {
                iterator++;
                pos++;
                c = *iterator;
                i++;
                if (c == '\n')
                    ln++;
            }

            iterator++;
            i++;
            pos++;
            Token token{};
            token.line = currentLine;
            token.type = STRING;
            token.associatedError = ParseError::INVALID_STRING;

            token.string = std::string_view{currentIteration, iterator};

            int subtractFromRange = static_cast<int>(i - currentLength - token.string.length());
            token.range = {currentPos, pos - (currentPos - subtractFromRange)};

            this->tokenList.push_back(token);
            iterator--;
            i--;
            pos--;
            continue;
        }

        if (c == '/' && *std::next(iterator) == '/') {
            int currentLength = i;
            int currentPos = pos;
            auto currentIteration = iterator;

            while (c != '\n') {
                c = *iterator;
                pos++;
                i++;
                iterator++;
            }
            iterator--;
            i--;
            pos--;

            Token token{};
            token.line = ln;
            token.type = COMMENT;

            token.string = std::string_view{currentIteration, iterator};

            int subtractFromRange = static_cast<int>(i - currentLength - token.string.length());
            token.range = {currentPos, pos - (currentPos - subtractFromRange)};

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        if (c == '@') {
            int currentLength = i;
            auto currentIteration = iterator;
            int currentPos = pos;

            while (c != '\n' && c != '\t' && c != '\r' && c != ' ' && c != '(') {
                c = *iterator;
                pos++;
                i++;
                iterator++;
            }
            iterator--;
            i--;
            pos--;

            if (c == '\n')
                ln++;
            Token token;
            token.line = ln;
            token.type = DEFINITION;
            token.associatedError = ParseError::INVALID_DEFINITION;

            int newStrLength = 0;
            token.string = std::string_view{currentIteration, iterator};

            int subtractFromRange = (i - currentLength - newStrLength);
            token.range = {currentPos, pos - (currentPos - subtractFromRange)};

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        if (std::isdigit(c) || (c == '-' && std::isdigit(*std::next(iterator)))) {
            auto currentIteration = iterator;
            int currentPos = pos;

            if (c == '-') {
                iterator++;
                pos++;
                i++;
                c = *iterator;
            }

#ifdef FGDPP_UNIFIED_FGD
            while (std::isdigit(c) || c == '.')
#else
            while (std::isdigit(c))
#endif
            {
                c = *iterator;
                i++;
                pos++;
                iterator++;
            }

            iterator--;
            i--;
            pos--;

            Token token;
            token.line = ln;
            token.type = NUMBER;
            token.associatedError = ParseError::INVALID_NUMBER;

            token.string = std::string_view{currentIteration, iterator};

            token.range = {currentPos, pos};

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        if (const char* valueKey = std::strchr(singleTokens, c)) {
            int spaces = (int)((int)((char*) valueKey - (char*) singleTokens) / sizeof(char)); // char should be 1, but I am sanity checking it anyway.
            TokenType tType = valueTokens[spaces];
            enum ParseError tParseError = tokenErrors[spaces];
            Token token;
            token.line = ln;
            token.type = tType;
            token.associatedError = tParseError;

            token.string = std::string_view{iterator, std::next(iterator)};

			token.range = {pos, pos + 1};

            this->tokenList.push_back(token);

            continue;
        }

        if (c != ' ') {
            int currentLength = i;
            auto currentIteration = iterator;
            int currentPos = pos;

            while (c != '\n' && c != ' ' && c != '\t' && c != '\r' && !std::strchr(singleTokens, c)) {
                iterator++;
                pos++;
                c = *iterator;
                i++;
            }

            Token token;
            token.line = ln;
            token.type = LITERAL;
            token.associatedError = ParseError::INVALID_LITERAL;

            token.string = std::string_view{currentIteration, iterator};

            int subtractFromRange = static_cast<int>(i - currentLength - token.string.length());
            token.range = {currentPos, pos - (currentPos - subtractFromRange)};

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }
    }

    return true;
}

FGD::FGD(std::string_view path, bool parseIncludes) {
	std::ifstream file{path.data(), std::ios::binary};
	if (!file.is_open()) {
		this->parseError = {ParseError::FAILED_TO_OPEN, 0, {0, 0}};
		return;
	}

	auto fileSize = static_cast<std::streamsize>(std::filesystem::file_size(path));
	this->rawFGDFile = std::string(fileSize, ' ');
	file.read(this->rawFGDFile.data(), fileSize);
	file.close();

	if (parseIncludes) {
		std::vector<std::string> exclusionList;
		exclusionList.emplace_back(path.data());

		std::string_view dirPath = path.substr(0, path.find_last_of('/'));

        for(auto itr = rawFGDFile.begin(); itr != rawFGDFile.end(); itr++)
        {
            if(*itr != '@')
                continue;

            if(std::string_view(itr, itr + 8) != "@include")
                continue;

            auto incStart = itr;

            itr += 8;
            while (*itr != '"')
            {
                if(itr == rawFGDFile.end()) {
                    this->parseError = {ParseError::PREMATURE_EOF, 0, {0, static_cast<int>(rawFGDFile.length())}};
                    return;
                }
                
                itr++;
            }
            itr++;
            auto strStart = itr;
            while (*itr != '"')
            {
                if(itr == rawFGDFile.end()) {
                    this->parseError = {ParseError::PREMATURE_EOF, 0, {0, static_cast<int>(rawFGDFile.length())}};
                    return;
                }
                itr++;
            }

            std::string currentPath = std::string(dirPath) + '/' + std::string (strStart, itr);

            if (std::find_if(exclusionList.begin(), exclusionList.end(), [currentPath](const std::string& v) {
				return v == currentPath;
			}) != exclusionList.end()) {
                auto dist = std::distance(this->rawFGDFile.begin(), incStart);
                this->rawFGDFile = this->rawFGDFile.replace(incStart, itr, "");
                itr = this->rawFGDFile.begin() + dist;
				continue;
			}

            exclusionList.push_back(currentPath);
            file.open(currentPath, std::ios::binary);
			if (!file.is_open()) continue;

			auto includeSize = static_cast<std::streamsize>(std::filesystem::file_size(currentPath));
			std::string includeFileContents(includeSize, ' ');
			file.read(includeFileContents.data(), includeSize);
			file.close();
            auto dist = std::distance(this->rawFGDFile.begin(), incStart);
            this->rawFGDFile = this->rawFGDFile.replace(incStart, itr, includeFileContents);
            itr = this->rawFGDFile.begin() + dist;
        }
	}

	if (!this->TokenizeFile()) {
		this->parseError = {ParseError::TOKENIZATION_ERROR, 0, {0, 0}};
		return;
	}

	if (!this->ParseFile()) {
//		this->parseError = {ParseError::PARSING_ERROR, 0, {0, 0}};
		return;
	}
}

#define ErrorHandle(iter) this->parseError = (iter == this->tokenList.cend()) ? (ParsingError{ParseError::PREMATURE_EOF, lastLine, {0, 0}}) : (ParsingError{iter->associatedError, iter->line, {iter->range.start, iter->range.end}}); return false

#define Forward(iterator, failureResult)            \
	do {                                            \
		iterator++;                                 \
		if (iterator == this->tokenList.end())      \
		    failureResult;                          \
		while (iterator->type == COMMENT) {         \
		    iterator++;                             \
		    if (iterator == this->tokenList.cend()) \
				failureResult;                      \
		}                                           \
	} while (0)


#ifdef FGDPP_UNIFIED_FGD
bool FGD::TagListDelimiter(std::vector<Token>::const_iterator& iter, TagList& tagList) {
    std::vector<UtilStringView> fields;

    int i = 0;
    bool hasPlus = false;
    while (iter->type == LITERAL || iter->type == PLUS || iter->type == COMMA || iter->type == STRING || iter->type == NUMBER) {
        if (iter->type == PLUS) {
            hasPlus = true;
            Forward(iter, return false);
            continue;
        }

        if (iter->type == COMMA) {
            tagList.tags.resize(i + 1);
            for (int j = 0; j < i; j++) {
                tagList.tags[j] = fields[j];
            }

            i = 0;
            Forward(iter, return false);
            continue;
        }

        if (!hasPlus) {
            fields.resize(i+1);
            fields[i] = iter->string;
            i++;
        } else {
            std::string t{"+"};
            t.append(( iter )->string);
            fields.resize(i + 1);
            //One of the very few times we need to copy.
            fields[i] = t;
            i++;
        }

        Forward(iter, return false);
    }

    if (i > 0) {
        tagList.tags.resize(i + 1);
        for (int j = 0; j < i; j++) {
            tagList.tags[j] = fields[j];
        }
    }

    if (iter->type != CLOSE_BRACKET)
        return false;

    return true;
}
#endif

bool FGD::ParseFile() {
    if (this->tokenList.empty())
        return false;

    int lastLine = this->tokenList[this->tokenList.size() - 1].line;
    for (auto iter = this->tokenList.cbegin(); iter != this->tokenList.cend(); iter++) {
        if (iter->type != DEFINITION)
            continue;


        if (iequals(iter->string, R"(@mapsize)")) {
            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != OPEN_PARENTHESIS) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != NUMBER) {
                ErrorHandle(iter);
            }

            this->FGDFileContents.mapSize.start = std::stoi(iter->string.get_string());

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != COMMA) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != NUMBER) {
                ErrorHandle(iter);
            }

            this->FGDFileContents.mapSize.end = std::stoi(iter->string.get_string());

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != CLOSE_PARENTHESIS) {
                ErrorHandle(iter);
            }

            continue;
        }

        if (iequals(iter->string, R"(@autovisgroup)")) {
            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != EQUALS) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != STRING) {
                ErrorHandle(iter);
            }

            AutoVisGroup visGroup;
            visGroup.name = iter->string;

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != OPEN_BRACKET) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != STRING && iter->type != CLOSE_BRACKET) {
                ErrorHandle(iter);
            }

            while (iter->type == STRING) {
                AutoVisGroupChild visGroupChild;
                visGroupChild.name = iter->string;

                Forward(iter, { ErrorHandle(iter); });
                if (iter->type != OPEN_BRACKET) {
                    ErrorHandle(iter);
                }

                Forward(iter, { ErrorHandle(iter); });
                if (iter->type != STRING && iter->type != CLOSE_BRACKET) {
                    ErrorHandle(iter);
                }

                while (iter->type == STRING) {
                    if (iter->type != STRING) {
                        ErrorHandle(iter);
                    }

                    visGroupChild.children.push_back(iter->string);

                    Forward(iter, { ErrorHandle(iter); });
                }

                visGroup.children.push_back(visGroupChild);

                if (iter->type != CLOSE_BRACKET) {
                    ErrorHandle(iter);
                }

                Forward(iter, { ErrorHandle(iter); });
            }

            if (iter->type != CLOSE_BRACKET) {
                ErrorHandle(iter);
            }

            this->FGDFileContents.autoVisGroups.push_back(visGroup);
            continue;
        }

        if (iequals(iter->string, R"(@include)")) {
            Forward(iter, { ErrorHandle(iter); });

            if (iter->type != STRING) {
                ErrorHandle(iter);
            }

            this->FGDFileContents.includes.push_back(iter->string);
            continue;
        }

        if (iequals(iter->string, R"(@MaterialExclusion)")) {
            Forward(iter, { ErrorHandle(iter); });

            if (iter->type != OPEN_BRACKET) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });

            while (iter->type == STRING) {
                this->FGDFileContents.materialExclusions.push_back( iter->string );

                Forward(iter, { ErrorHandle(iter); });
            }

            if (iter->type != CLOSE_BRACKET) {
                ErrorHandle(iter);
            }

            continue;
        }

        if (iter->string.ends_with("Class")) {
            Entity entity;
            entity.type = iter->string;

            Forward(iter, { ErrorHandle(iter); });

            while (iter->type == LITERAL) {
                ClassProperties classProperties;
                classProperties.name = iter->string;

                Forward(iter, { ErrorHandle(iter); });
                if (iter->type == OPEN_PARENTHESIS) {
                    // if there are more than 64 non comma separated parameters, you're doing something wrong.
                    // The value is already so high in case anyone adds new fgd class parameters in the future that require them.
					static constexpr int MAX_FIELDS = 64;

                    std::string_view fields[MAX_FIELDS];
                    int i = 0;

                    Forward(iter, { ErrorHandle(iter); });
                    while (iter->type == LITERAL || iter->type == COMMA || iter->type == STRING || iter->type == NUMBER) {
                        if (i > MAX_FIELDS) {
							// wtf happened?
                            ErrorHandle(iter);
                        }

                        if (iter->type == COMMA) {
                            ClassProperty property;
                            for (int j = 0; j < i; j++) {
                                property.properties.emplace_back(fields[j]);
                            }

                            i = 0;
                            Forward(iter, { ErrorHandle(iter); });
                            classProperties.classProperties.push_back(property);
                            continue;
                        }

                        fields[i] = iter->string;
                        i++;

                        Forward(iter, { ErrorHandle(iter); });
                    }

                    if (i > 0) {
                        ClassProperty property;
                        for (int j = 0; j < i; j++) {
                            property.properties.emplace_back(fields[j]);
                        }

                        i = 0;
                        Forward(iter, { ErrorHandle(iter); });
                        classProperties.classProperties.push_back(property);
                        entity.classProperties.push_back(classProperties);
                        continue;
                    }

                    if (iter->type != CLOSE_PARENTHESIS) {
                        ErrorHandle(iter);
                    }

                    Forward(iter, { ErrorHandle(iter); });
                }

                entity.classProperties.push_back(classProperties);
            }

            if (iter->type != EQUALS) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            if (iter->type != LITERAL) {
                ErrorHandle(iter);
            }

            entity.entityName = iter->string;

            Forward(iter, { ErrorHandle(iter); });

            if (iter->type == COLUMN) {
                Forward(iter, { ErrorHandle(iter); });

                if (iter->type != STRING) {
                    ErrorHandle(iter);
                }

                while (iter->type == STRING) {
#ifndef FGDPP_UNIFIED_FGD
                    if (iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH) {
						ErrorHandle(iter);
					}
#endif
                    entity.entityDescription.push_back(iter->string);
                    Forward(iter, { ErrorHandle(iter); });
                    if (iter->type == PLUS) {
                        Forward(iter, { ErrorHandle(iter); });
                    }
                }
            }

            if (iter->type != OPEN_BRACKET) {
                ErrorHandle(iter);
            }

            Forward(iter, { ErrorHandle(iter); });
            while (iter->type != CLOSE_BRACKET) {
#ifdef FGDPP_UNIFIED_FGD
				if (iequals(iter->string, R"(@resources)")) {
					Forward(iter, { ErrorHandle(iter); });

					if (iter->type != OPEN_BRACKET) {
						ErrorHandle(iter);
					}

					Forward(iter, { ErrorHandle(iter); });

					while (iter->type != CLOSE_BRACKET) {
						if (iter->type != LITERAL) {
							ErrorHandle(iter);
						}

						EntityResource resource;
						resource.key = ( iter->string );

						Forward(iter, { ErrorHandle(iter); });
						resource.value = ( iter->string );

						Forward(iter, { ErrorHandle(iter); });
						if (iter->type == OPEN_BRACKET) {
							Forward(iter, { ErrorHandle(iter); });
							if (!TagListDelimiter(iter, resource.tagList)) {
								ErrorHandle(iter);
							}
							Forward(iter, { ErrorHandle(iter); });
						}
					}

					if (iter->type != CLOSE_BRACKET) {
						ErrorHandle(iter);
					}

					Forward(iter, { ErrorHandle(iter); });
					continue;
				}
#endif

                if (iter->type != LITERAL) {
                    ErrorHandle(iter);
                }

                if (iequals(iter->string, R"(input)") || iequals(iter->string, R"(output)")) {
                    InputOutput inputOutput;
                    inputOutput.putType = iequals(iter->string, R"(input)") == 0 ? IO::INPUT : IO::OUTPUT;

                    Forward(iter, { ErrorHandle(iter); });

                    inputOutput.name = iter->string;

                    Forward(iter, { ErrorHandle(iter); });

#ifdef FGDPP_UNIFIED_FGD
                    if (iter->type == OPEN_BRACKET) {
						Forward(iter, { ErrorHandle(iter); });
						if (!TagListDelimiter(iter, inputOutput.tagList)) {
							ErrorHandle(iter);
						}
						Forward(iter, { ErrorHandle(iter); });
					}
#endif

                    if (iter->type != OPEN_PARENTHESIS) {
                        ErrorHandle(iter);
                    }

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type != LITERAL) {
                        ErrorHandle(iter);
                    }

                    int index = 0;
                    while (index < 9) {
                        if (iequals(typeStrings[index], iter->string))
                            break;
                        index++;
                    }
                    if (index == 9)
                        inputOutput.type = EntityIOPropertyType::t_custom;
                    else
                        inputOutput.type = typeList[index];

                    inputOutput.stringType = iter->string;

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type != CLOSE_PARENTHESIS) {
                        ErrorHandle(iter);
                    }

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type == COLUMN) {
                        Forward(iter, { ErrorHandle(iter); });

                        if (iter->type != STRING) {
                            ErrorHandle(iter);
                        }

                        while (iter->type == STRING) {
#ifndef FGDPP_UNIFIED_FGD
                            if (iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH) {
								ErrorHandle(iter);
							}
#endif
                            inputOutput.description.push_back(iter->string);
                            Forward(iter, { ErrorHandle(iter); });
                            if (iter->type == PLUS) {
                                Forward(iter, { ErrorHandle(iter); });
                            }
                        }
                    }
                    entity.inputOutput.push_back(inputOutput);
                    continue;
                } else {
                    EntityProperties& entityProperties = entity.entityProperties.emplace_back();
                    entityProperties.readOnly = false;
                    entityProperties.reportable = false;
                    entityProperties.propertyName = iter->string;
                    Forward(iter, { ErrorHandle(iter); });

#ifdef FGDPP_UNIFIED_FGD
                    if (iter->type == OPEN_BRACKET) {
						Forward(iter, { ErrorHandle(iter); });
						if (!TagListDelimiter(iter, entityProperties.tagList)) {
							ErrorHandle(iter);
						}
						Forward(iter, { ErrorHandle(iter); });
					}
#endif

                    if (iter->type != OPEN_PARENTHESIS) {
                        ErrorHandle(iter);
                    }

                    Forward(iter, { ErrorHandle(iter); });
                    if (iter->type != LITERAL) {
                        ErrorHandle(iter);
                    }

                    entityProperties.type = iter->string;

                    Forward(iter, { ErrorHandle(iter); });
                    if (iter->type != CLOSE_PARENTHESIS) {
                        ErrorHandle(iter);
                    }

                    Forward(iter, { ErrorHandle(iter); });
                    if (iequals(iter->string, R"(readonly)")) {
                        entityProperties.readOnly = true;
                        Forward(iter, { ErrorHandle(iter); });
                    }

                    if (iequals(iter->string, R"(*)") || iequals(iter->string, R"(report)")) {
                        entityProperties.reportable = true;
                        Forward(iter, { ErrorHandle(iter); });
                    }

                    if (iter->type == EQUALS) {
                        goto isFOC;
                    }

                    if (iter->type != COLUMN)
                        continue;

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type != STRING) {
                        ErrorHandle(iter);
                    }

                    entityProperties.displayName = iter->string;

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type == EQUALS) {
                        goto isFOC;
                    }

                    if (iter->type != COLUMN)
                        continue;

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type != COLUMN) {
                        entityProperties.defaultValue = iter->string;
                        Forward(iter, { ErrorHandle(iter); });
                    }

                    if (iter->type == EQUALS) {
                        goto isFOC;
                    }

                    if (iter->type != COLUMN)
                        continue;

                    Forward(iter, { ErrorHandle(iter); });

                    if (iter->type != STRING) {
                        ErrorHandle(iter);
                    }

                    while (iter->type == STRING) {
#ifndef FGDPP_UNIFIED_FGD
                        if (iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH) {
							ErrorHandle(iter);
						}
#endif
                        entityProperties.propertyDescription.push_back(iter->string);
                        Forward(iter, { ErrorHandle(iter); });
                        if (iter->type == PLUS) {
                            Forward(iter, { ErrorHandle(iter); });
                        }
                    }

					/*
					if (!ProcessFGDStrings(iter, &entityProperties->propertyDescription)) {
						ErrorHandle(iter);
					}
					*/

                    if (iter->type == EQUALS) {
                        goto isFOC;
                    }

                    continue;

                    isFOC:
                    {
                        bool isFlags = iequals(entityProperties.type, R"(flags)");

                        Forward(iter, { ErrorHandle(iter); });
                        if (iter->type != OPEN_BRACKET) {
                            ErrorHandle(iter);
                        }

                        Forward(iter, { ErrorHandle(iter); });
                        while (iter->type != CLOSE_BRACKET) {
                            if (isFlags && iter->type != NUMBER) {
                                ErrorHandle(iter);
                            }

                            if (isFlags) {
                                Flag& flags = entityProperties.flags.emplace_back();
                                flags.value = std::stoi(iter->string.operator std::string());

                                Forward(iter, { ErrorHandle(iter); });
                                if (iter->type != COLUMN) {
                                    ErrorHandle(iter);
                                }

                                Forward(iter, { ErrorHandle(iter); });
                                if (iter->type != STRING) {
                                    ErrorHandle(iter);
                                }

                                flags.displayName = iter->string;

                                if (std::next(iter)->type == COLUMN) {
                                    Forward(iter, { ErrorHandle(iter); });

                                    Forward(iter, { ErrorHandle(iter); });
                                    if ( iter->type != NUMBER ) {
                                        ErrorHandle(iter);
                                    }
                                    flags.checked = iter->string == "1";

#ifdef FGDPP_UNIFIED_FGD
                                    if (std::next(iter)->type == OPEN_BRACKET) {
										Forward(iter, { ErrorHandle(iter); });
										Forward(iter, { ErrorHandle(iter); });
										if (!TagListDelimiter(iter, flags.tagList)) {
											ErrorHandle(iter);
										}
									}
#endif
                                }

                                Forward(iter, { ErrorHandle(iter); });
                            } else {
                                Choice& choice = entityProperties.choices.emplace_back();
                                choice.value = iter->string;

                                Forward(iter, { ErrorHandle(iter); });
                                if ( iter->type != COLUMN ) {
                                    ErrorHandle(iter);
                                }

                                Forward(iter, { ErrorHandle(iter); });
                                if ( iter->type != STRING ) {
                                    ErrorHandle(iter);
                                }

                                choice.displayName = iter->string;

#ifdef FGDPP_UNIFIED_FGD
                                if (std::next(iter)->type == OPEN_BRACKET) {
									Forward(iter, { ErrorHandle(iter); });
									Forward(iter, { ErrorHandle(iter); });

									if (!TagListDelimiter(iter, choice.tagList)) {
										ErrorHandle(iter);
									}
								}
#endif

                                Forward(iter, { ErrorHandle(iter); });
                            }
                        }
                    }
                }

                Forward(iter, { ErrorHandle(iter); });
            }

            this->FGDFileContents.entities.push_back(entity);
        }
    }

    return true;
}

UtilStringView FGD::Write() const {

    auto& fileContents = this->FGDFileContents;
    UtilStringView fgd = UtilStringView::make_self_owning("@mapsize(");
    fgd << std::to_string(fileContents.mapSize.start) << "," << std::to_string(fileContents.mapSize.end) <<")\n\n";

    for(const auto& groups : fileContents.autoVisGroups)
    {
       fgd << "@AutoVisGroup = " << groups.name << "\n\t[\n";
       for(const auto& childGroup : groups.children)
       {
           fgd << "\t" << childGroup.name << "\n\t\t[\n";

           for(const auto& contents : childGroup.children)
               fgd << "\t\t" << contents << "\n";

           fgd << "\t\t]\n";
       }
        fgd << "\t]\n";
    }

    for(const auto& entity : fileContents.entities)
    {
        fgd << entity.type << " ";
        for(const auto& props : entity.classProperties)
        {
            fgd << props.name << "( ";

            for(const auto& subProps : props.classProperties)
                for(const auto& subsubprops : subProps.properties)
                    fgd << subsubprops << " ";

            fgd << ")\n\t";
        }
        fgd << "= " << entity.entityName;
        if(!entity.entityDescription.empty())
            fgd << " : ";
        for(auto descs = entity.entityDescription.begin(); descs != entity.entityDescription.end(); descs++ )
            fgd << *descs << ((std::next(descs) != entity.entityDescription.end()) ? " + " : "");
        fgd << "\n\t[\n";

        for(const auto& entProps : entity.entityProperties)
            {
                fgd << "\t" << entProps.propertyName << "(" << entProps.type << ") ";

                if(entProps.readOnly)
                    fgd << " readonly ";

                if(entProps.reportable)
                    fgd << " reportable ";

                if(entProps.displayName.length() > 0)
                    fgd << ": " << entProps.displayName;


                if(entProps.displayName.length() <= 0 && entProps.defaultValue.length() > 0)
                    fgd << " :";


                if(entProps.defaultValue.length() > 0)
                    fgd << " : " << entProps.defaultValue;


                if(entProps.defaultValue.length() <= 0 && !entProps.propertyDescription.empty())
                    fgd << " :";

                if(!entProps.propertyDescription.empty())
                    fgd << " : ";

                for(auto descs = entProps.propertyDescription.begin(); descs != entProps.propertyDescription.end(); descs++ )
                    fgd << *descs << ((std::next(descs) != entProps.propertyDescription.end()) ? " + " : "");

                if(entProps.type == "choices")
                {
                    fgd << " = \n\t\t[\n";

                    for(const auto &choice : entProps.choices)
                    {
                        fgd << "\t\t" << choice.value << (choice.displayName.length() > 0 ? ( UtilStringView("") << " : " << choice.displayName) : "") << "\n";
                    }
                    fgd << "\t\t]\n";
                }

                if(entProps.type == "flags")
                {
                    fgd << " = \n\t\t[\n";

                    for(const auto &flags : entProps.flags)
                    {
                        fgd << "\t\t" << std::to_string(flags.value) << (flags.displayName.length() > 0 ? ( UtilStringView("") << " : " << flags.displayName) : " : ") << (flags.checked ? " : 0" : " : 1") << "\n";
                    }
                    fgd << "\t\t]\n";
                }

                fgd << "\n";
            }

        if(!entity.entityProperties.empty())
            fgd << "\n";

        std::vector<const InputOutput*> inputs;
        std::vector<const InputOutput*> output;

        for(auto& entIO : entity.inputOutput)
        {
            if(entIO.putType == IO::INPUT)
                inputs.push_back(&entIO);
            else
                output.push_back(&entIO);
        }

        if(!inputs.empty())
        {
            fgd << "\n\t // Inputs\n";
        }

        for(const auto entIO : inputs)
        {
            fgd << "\t"
                << "input "
                << entIO->name << "("
                << entIO->stringType
                << ")";
            if(!entIO->description.empty())
            {
                fgd << " : ";
                for(auto descs = entIO->description.begin(); descs != entIO->description.end(); descs++ )
                    fgd << *descs << ((std::next(descs) != entIO->description.end()) ? " + " : "");
            }
            fgd << "\n";
        }

        if(!output.empty())
        {
            fgd << "\n\t // Outputs\n";
        }

        for(const auto entIO : output)
        {
            fgd << "\t"
            << "output "
            << entIO->name << "("
            << entIO->stringType
            << ")";
            if(!entIO->description.empty())
            {
                fgd << " : ";
                for(auto descs = entIO->description.begin(); descs != entIO->description.end(); descs++ )
                    fgd << *descs << ((std::next(descs) != entIO->description.end()) ? " + " : "");
            }
            fgd << "\n";
        }

        fgd << "\n\t]\n\n";


    }

    return fgd;



}
