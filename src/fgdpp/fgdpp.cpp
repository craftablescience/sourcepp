#include <cstring>
#include <fstream>
#include <iostream>
#include <regex>
#include "fgdpp/fgdpp.h"

#define FGDPP_MAX_STR_CHUNK_LENGTH 1024

using namespace fgdpp;

constexpr char singleTokens[] = "{}[](),:=+";
constexpr FGDParser::TokenType valueTokens[] = {FGDParser::OPEN_BRACE, FGDParser::CLOSE_BRACE, FGDParser::OPEN_BRACKET, FGDParser::CLOSE_BRACKET, FGDParser::OPEN_PARENTHESIS, FGDParser::CLOSE_PARENTHESIS, FGDParser::COMMA, FGDParser::COLUMN, FGDParser::EQUALS, FGDParser::PLUS };
constexpr enum ParseError tokenErrors[] = { INVALID_OPEN_BRACE, INVALID_CLOSE_BRACE, INVALID_OPEN_BRACKET, INVALID_CLOSE_BRACKET, INVALID_OPEN_PARENTHESIS, INVALID_CLOSE_PARENTHESIS, INVALID_COMMA,INVALID_COLUMN,INVALID_EQUALS,INVALID_PLUS};

std::string_view typeStrings[9] = { "string", "integer", "float", "bool", "void", "script", "vector", "target_destination", "color255" };
EntityIOPropertyType typeList[9] = { EntityIOPropertyType::t_string, EntityIOPropertyType::t_integer, EntityIOPropertyType::t_float, EntityIOPropertyType::t_bool, EntityIOPropertyType::t_void, EntityIOPropertyType::t_script, EntityIOPropertyType::t_vector, EntityIOPropertyType::t_target_destination, EntityIOPropertyType::t_color255 };

bool ichar_equals(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}

bool iequals(std::string_view lhs, std::string_view rhs)
{
    return std::ranges::equal(lhs, rhs, ichar_equals);
}


bool FGDParser::TokenizeFile()
{

    if(this->rawFGDFile.empty())
        return false;

    int pos = 1, ln = 1, i = 0;

    for ( auto iterator = this->rawFGDFile.cbegin(); iterator != this->rawFGDFile.cend(); iterator++, i++, pos++ )
    {
        char c = *iterator;

        if ( c == '\t' )
            continue;

        if ( c == '\r' )
            continue;

        if ( c == '\n' )
        {
            ln++;
            pos = 1;
            continue;
        }

        if ( c == '"' )
        {
            int currentLine = ln;
            int currentLength = i;
            int currentPos = pos;
            auto currentIteration = iterator;

            c = '\t'; // We can get away with this to trick the while loop :)
            while ( c != '"' )
            {
                iterator++;
                pos++;
                c = *iterator;
                i++;
                if ( c == '\n' )
                    ln++;
            }

            iterator++;
            i++;
            pos++;
            Token token {};
            token.line = currentLine;
            token.type = STRING;
            token.associatedError = INVALID_STRING;

            int newStrLength = 0;
            token.string = std::string_view{currentIteration, iterator };

            int subtractFromRange = (i - currentLength - token.string .length());

            Range range = { currentPos, pos - (currentPos - subtractFromRange ) };
            token.range = range;


            this->tokenList.push_back(token);
            iterator--;
            i--;
            pos--;
            continue;
        }

        if ( c == '/' && *std::next( iterator ) == '/' )
        {
            int currentLength = i;
            int currentPos = pos;
            auto currentIteration = iterator;

            while ( c != '\n' )
            {
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

            token.string = std::string_view {currentIteration, iterator};

            int subtractFromRange = (i - currentLength - token.string .length());

            Range range = { currentPos, pos - (currentPos - subtractFromRange ) };
            token.range = range;

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        if ( c == '@' )
        {
            int currentLength = i;
            auto currentIteration = iterator;
            int currentPos = pos;

            while ( c != '\n' && c != '\t' && c != '\r' && c != ' ' && c != '(' )
            {
                c = *iterator;
                pos++;
                i++;
                iterator++;
            }
            iterator--;
            i--;
            pos--;

            if ( c == '\n' )
                ln++;
            Token token;
            token.line = ln;
            token.type = DEFINITION;
            token.associatedError = INVALID_DEFINITION;

            int newStrLength = 0;
            token.string = std::string_view{currentIteration, iterator};

            int subtractFromRange = (i - currentLength - newStrLength);

            Range range = { currentPos, pos - (currentPos - subtractFromRange ) };
            token.range = range;

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        if ( isdigit( c ) != 0 || ( c == '-' && isdigit( *std::next(iterator) ) ) )
        {
            int currentLength = i;
            auto currentIteration = iterator;
            int currentPos = pos;

            if ( c == '-' )
            {
                iterator++;
                pos++;
                i++;
                c = *iterator;
            }

#ifdef FGDPP_UNIFIED_FGD
            while ( isdigit( c ) != 0 || c == '.' )
#else
            while ( isdigit( c ) != 0 )
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
            token.associatedError = INVALID_NUMBER;
            Range range = { currentPos, pos };
            token.range = range;
            token.string = std::string_view{currentIteration, iterator};

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

        const char *valueKey = strchr( singleTokens, c );

        if ( valueKey )
        {
            int spaces = (int)( (int)( (char *)valueKey - (char *)singleTokens ) / sizeof( char ) ); // char should be 1, but I am sanity checking it anyway.
            TokenType tType = valueTokens[spaces];
            enum ParseError tParseError = tokenErrors[spaces];
            Token token;
            token.line = ln;
            token.type = tType;
            token.associatedError = tParseError;
            Range range = { pos, pos + 1 };
            token.range = range;

            token.string = std::string_view { iterator, std::next(iterator) };

            this->tokenList.push_back(token);

            continue;
        }

        if ( c != ' ' )
        {
            int currentLength = i;
            auto currentIteration = iterator;
            int currentPos = pos;

            while ( c != '\n' && c != ' ' && c != '\t' && c != '\r' && !strchr( singleTokens, c ) )
            {
                iterator++;
                pos++;
                c = *iterator;
                i++;
            }

            Token token;
            token.line = ln;
            token.type = LITERAL;
            token.associatedError = INVALID_LITERAL;

            token.string = std::string_view {currentIteration, iterator};

            int subtractFromRange = (i - currentLength - token.string.length());

            Range range = { currentPos, pos - (currentPos - subtractFromRange ) };
            token.range = range;

            this->tokenList.push_back(token);

            iterator--;
            i--;
            pos--;
            continue;
        }

    }

    return true;
}

fgdpp::FGDParser::FGDParser(std::string_view path, bool parseIncludes) {

    std::ifstream file;
    file.open(path.data());

    if (file.is_open())
    {
        file.seekg (0, std::ios::end);
        size_t size = file.tellg();
        this->rawFGDFile = std::string(size, ' ');
        file.seekg (0, std::ios::beg);
        file.read ( this->rawFGDFile.data(), size);
        file.close();

        if(parseIncludes) {
            std::vector<std::string> exclusionList{};
            exclusionList.emplace_back(path.data());

            std::string_view dirPath = path.substr(0, path.find_last_of('/'));
            std::smatch match;
            std::regex exr{"@include+ \"(.*)\""};

            while (std::regex_search( this->rawFGDFile, match, exr)) {
                std::regex thisInclude("@include+ \"" + match[1].str() + "\"");

                std::string currentPath = dirPath.data() + match[1].str();

                if (std::find_if(exclusionList.begin(), exclusionList.end(),
                                 [currentPath](const std::string &v) { return v == currentPath; }) !=
                    exclusionList.end()) {
                    std::cout << match[1].str() + " Already included." << std::endl;
                     this->rawFGDFile = std::regex_replace( this->rawFGDFile, thisInclude, "");
                    continue;
                }

                std::cout << match[1].str() << std::endl;

                exclusionList.push_back(currentPath);

                file.open(std::string(dirPath) + "/" + match[1].str());
                if (!file.is_open()) return;

                file.seekg(0, std::ios::end);
                size_t includeSize = file.tellg();
                std::string includeFileContents(includeSize, ' ');
                file.seekg(0, std::ios::beg);
                file.read(includeFileContents.data(), includeSize);
                file.close();

                 this->rawFGDFile = std::regex_replace( this->rawFGDFile, thisInclude, includeFileContents,
                                                  std::regex_constants::format_first_only);

            }

        }

        (std::remove( this->rawFGDFile.begin(),  this->rawFGDFile.end(), '\r'));

        bool tokeniseFIle = this->TokenizeFile();
//        std::cout << tokeniseFIle << "\n";
        if(!tokeniseFIle) return;

        bool parseFile = this->ParseFile();
//        std::cout << parseFile << "\n";
        if(!parseFile) return;
    } else
        this->parseError = {FAILED_TO_OPEN, 0, {0,0}};

}

#define ErrorHandle(iter) this->parseError = (iter == this->tokenList.cend()) ? (ParsingError{ PREMATURE_EOF, lastLine, { 0, 0 } }) : (ParsingError{ iter->associatedError, iter->line, { iter->range.start, iter->range.end} }); return false

#define Forward( iterator, failureResult )      \
	iterator++;                        \
	if ( iterator == this->tokenList.end() )    \
		failureResult;                          \
	while ( iterator->type == COMMENT )       \
	{                                           \
		iterator++;                             \
		if ( iterator == this->tokenList.cend() )\
			failureResult;                      \
	}


#ifdef FGDPP_UNIFIED_FGD
bool FGDParser::TagListDelimiter(std::vector<Token>::const_iterator& iter, TagList &tagList )
{

    std::vector<std::string_view> fields;

    int i = 0;
    bool hasPlus = false;
    while ( iter->type == LITERAL || iter->type == PLUS || iter->type == COMMA || iter->type == STRING || iter->type == NUMBER )
    {
        if ( iter->type == PLUS )
        {
            hasPlus = true;
            Forward( iter, { return false; } );
            continue;
        }

        if ( iter->type == COMMA )
        {
            for ( int j = 0; j < i; j++ )
            {
                tagList.tags.resize(j+1);
                tagList.tags[j] = fields[j];
            }

            i = 0;
            Forward( ( iter ), return false );
            continue;
        }

        if ( !hasPlus )
        {
            fields.resize(i+1);
            fields[i] = ( ( iter )->string );
            i++;
        }
        else
        {
//            std::string t{"+"};
//            t.append(( iter )->string);
            //TODO: We ned to identify +, trust me, we do.
            fields.resize(i+1);
            fields[i] = ( ( iter )->string );
            i++;
        }

        Forward( ( iter ), return false );
    }

    if ( i > 0 )
    {
        for ( int j = 0; j < i; j++ )
        {
            tagList.tags.resize(j+1);
            tagList.tags[j] = fields[j];
        }
    }

    if ( iter->type != CLOSE_BRACKET )
        return false;

    return true;
}
#endif

bool FGDParser::ParseFile() {

    if(this->tokenList.empty())
        return false;
    int lastLine = this->tokenList[this->tokenList.size() - 1].line;
    for(auto iter = this->tokenList.cbegin(); iter != this->tokenList.cend(); iter++)
    {
        if ( iter->type != DEFINITION )
            continue;

        if(iequals(iter->string, "@mapsize"))
        {
            Forward( iter, { ErrorHandle(iter); } );
            if ( iter->type != OPEN_PARENTHESIS )
            {
                ErrorHandle(iter);
                
            }

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != NUMBER )
            {
                ErrorHandle(iter);
            }

            this->FGDFileContents.mapSize.start = std::stoi( iter->string.data() );

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != COMMA )
            {
                ErrorHandle(iter);
                
            }

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != NUMBER )
            {
                ErrorHandle(iter);
                
            }

            this->FGDFileContents.mapSize.end = std::stoi( iter->string.data() );

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != CLOSE_PARENTHESIS )
            {
                ErrorHandle(iter);
                
            }

            continue;
        }

        if(iequals(iter->string, "@AutoVisgroup"))
        {
            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != EQUALS )
            {
                ErrorHandle(iter);
                
            }

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != STRING )
            {
                ErrorHandle(iter);
                
            }

            AutoVIsGroup visGroup;

            visGroup.name = iter->string;

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != OPEN_BRACKET )
            {
                ErrorHandle(iter);
                
            }

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != STRING && iter->type != CLOSE_BRACKET )
            {
                ErrorHandle(iter);
                
            }

            while ( iter->type == STRING )
            {
                AutoVisGroupChild visGroupChild;

                visGroupChild.name = iter->string;

                Forward( iter, { ErrorHandle(iter); } );
                if ( iter->type != OPEN_BRACKET )
                {
                    ErrorHandle(iter);
                }

                Forward( iter, { ErrorHandle(iter);  } );
                if ( iter->type != STRING && iter->type != CLOSE_BRACKET )
                {
                    ErrorHandle(iter);
                }

                while ( iter->type == STRING )
                {
                    if ( iter->type != STRING )
                    {
                        ErrorHandle(iter);
                    }

                    visGroupChild.children.push_back(iter->string);

                    visGroup.children.push_back(visGroupChild);
                    Forward( iter, { ErrorHandle(iter);  } );
                }

                if ( iter->type != CLOSE_BRACKET )
                {
                    ErrorHandle(iter);
                    
                }

                Forward( iter, { ErrorHandle(iter);  } );
            }

            if ( iter->type != CLOSE_BRACKET )
            {
                ErrorHandle(iter);
                
            }

            this->FGDFileContents.autoVisGroups.push_back(visGroup);

            continue;
        }

        if(iequals(iter->string, "@include"))
        {
            Forward( iter, { ErrorHandle(iter);  } );

            if ( iter->type != STRING )
            {
                ErrorHandle(iter);
            }

            this->FGDFileContents.includes.push_back( iter->string );
            continue;
        }

        if(iequals(iter->string, "@MaterialExclusion"))
        {
            Forward( iter, { ErrorHandle(iter);  } );

            if ( iter->type != OPEN_BRACKET )
            {
                ErrorHandle(iter);
                
            }

            Forward( iter, { ErrorHandle(iter);  } );

            while ( iter->type == STRING )
            {
                this->FGDFileContents.materialExclusions.push_back( iter->string );

                Forward( iter, { ErrorHandle(iter);  } );
            }

            if ( iter->type != CLOSE_BRACKET )
            {
                ErrorHandle(iter);
                
            }

            continue;
        }

        if ( iter->string.ends_with( "Class" ) )
        {
            Entity entity;

            entity.type = iter->string;

            Forward( iter, { ErrorHandle(iter);  } );

            while ( iter->type == LITERAL )
            {
                ClassProperties classProperties;

                classProperties.name = iter->string;

                Forward( iter, { ErrorHandle(iter);  } );
                if ( iter->type == OPEN_PARENTHESIS )
                {
                    // if there are more than 40 non comma separated parameters, you're doing something wrong.
                    // The value is already so high in case anyone adds new fgd class parameters in the future that require them.
                    std::string_view fields[40];
                    int i = 0;

                    Forward( iter, { ErrorHandle(iter);  } );
                    while ( iter->type == LITERAL || iter->type == COMMA || iter->type == STRING || iter->type == NUMBER )
                    {
                        if ( i > 40 ) // wtf happened?
                        {
                            ErrorHandle(iter);
                        }

                        if ( iter->type == COMMA )
                        {
                            //AssignOrResizeArray( classProperties->classProperties, ClassProperty_t *, classProperties->classPropertyCount, { *err = ErrorFromValues( ALLOCATION_FAILURE, iter->line, iter->range.start, iter->range.end );  } );

                            ClassProperty property; // = classProperties->classProperties[classProperties->classPropertyCount - 1] = malloc( sizeof( ClassProperty_t ) );
//                            property.properties// = malloc( sizeof( char * ) * i );
                            for ( int j = 0; j < i; j++ )
                            {
                                property.properties.push_back(fields[j]);
                            }

                            i = 0;
                            Forward( iter, { ErrorHandle(iter);  } );
                            classProperties.classProperties.push_back(property);
                            continue;
                        }

                        fields[i] = iter->string;
                        i++;

                        Forward( iter, { ErrorHandle(iter);  } );
                    }

                    if ( i > 0 )
                    {
                        ClassProperty property;
                        for ( int j = 0; j < i; j++ )
                        {
                            property.properties.push_back(fields[j]);
                        }

                        i = 0;
                        Forward( iter, { ErrorHandle(iter); } );
                        classProperties.classProperties.push_back(property);
                        entity.classProperties.push_back(classProperties);
//                        std::cout << classProperties.name << std::endl;
                        continue;
                    }

                    if ( iter->type != CLOSE_PARENTHESIS )
                    {
                        ErrorHandle(iter);
                    }

                    Forward( iter, { ErrorHandle(iter);  } );
                }

//                std::cout << classProperties.name << std::endl;

                entity.classProperties.push_back(classProperties);
            }

            if ( iter->type != EQUALS )
            {
                ErrorHandle(iter);
            }

            Forward( iter, { ErrorHandle(iter);  } );
            if ( iter->type != LITERAL )
            {
                ErrorHandle(iter);
            }

            entity.entityName = iter->string;

            Forward( iter, { ErrorHandle(iter);  } );

            if ( iter->type == COLUMN )
            {
                Forward( iter, { ErrorHandle(iter); } );

                if ( iter->type != STRING )
                {
                    ErrorHandle(iter);
                    
                }

                while(iter->type == STRING)
                {
#ifndef FGDPP_UNIFIED_FGD
                    if(iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH){ ErrorHandle(iter); }
#endif
                    entity.entityDescription.push_back(iter->string);
                    Forward( iter, { ErrorHandle(iter); } );
                    if(iter->type == PLUS)
                    {
                        Forward( iter, { ErrorHandle(iter);  } );
                    }
                }

            }

            if ( iter->type != OPEN_BRACKET )
            {
                ErrorHandle(iter);
            }

            Forward( iter, { ErrorHandle(iter);  } );
            while ( iter->type != CLOSE_BRACKET )
            {
#ifdef FGDPP_UNIFIED_FGD
               // if ( strcasecmp( iter->string.data(), "@resources" ) == 0 )
				if(iequals(iter->string, "@resources"))
                {
					Forward( iter, { ErrorHandle(iter);  } );

					if ( iter->type != OPEN_BRACKET )
					{
						ErrorHandle(iter);
						
					}

					Forward( iter, { ErrorHandle(iter);  } );

					while ( iter->type != CLOSE_BRACKET )
					{
						if ( iter->type != LITERAL )
						{
							ErrorHandle(iter);
							
						}

						EntityResource resource;

						resource.key = ( iter->string );

						Forward( iter, { ErrorHandle(iter);  } );

						resource.value = ( iter->string );

						Forward( iter, { ErrorHandle(iter);  } );

						if ( iter->type == OPEN_BRACKET )
						{
							Forward( iter, { ErrorHandle(iter);  } );
							if ( !TagListDelimiter( iter, resource.tagList ) )
							{
								ErrorHandle(iter);
								
							}
							Forward( iter, { ErrorHandle(iter);  } );
						}
					}

					if ( iter->type != CLOSE_BRACKET )
					{
						ErrorHandle(iter);
						
					}

					Forward( iter, { ErrorHandle(iter);  } );
					continue;
				}
#endif

                if ( iter->type != LITERAL )
                {
                    ErrorHandle(iter);
                    
                }

                if(iequals(iter->string, "input") || iequals(iter->string, "output"))
                //if ( strcasecmp( , "input" ) == 0 || strcasecmp( iter->string.data(), "output" ) == 0 )
                {
                    InputOutput inputOutput;

                    inputOutput.putType = iequals(iter->string, "input") == 0 ? INPUT : OUTPUT;

                    Forward( iter, { ErrorHandle(iter);  } );

                    inputOutput.name = iter->string;

                    Forward( iter, { ErrorHandle(iter);  } );

#ifdef FGDPP_UNIFIED_FGD
                    if ( iter->type == OPEN_BRACKET )
					{
						Forward( iter, { ErrorHandle(iter);  } );
						if ( !TagListDelimiter( iter, inputOutput.tagList ) )
						{
							ErrorHandle(iter);
							
						}
						Forward( iter, { ErrorHandle(iter);  } );
					}
#endif

                    if ( iter->type != OPEN_PARENTHESIS )
                    {
                        ErrorHandle(iter);
                        
                    }

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type != LITERAL )
                    {
                        ErrorHandle(iter);
                        
                    }

                    int index = 0;
                    while ( index < 9 )
                    {
                        //if ( strcasecmp( typeStrings[index].data(), iter->string.data() ) == 0 )
                        //iequals(typeStrings[index], iter->string)
                        if(iequals(typeStrings[index], iter->string))
                            break;
                        index++;
                    }
                    if ( index == 9 )
                        inputOutput.type = t_custom;
                    else
                        inputOutput.type = typeList[index];

                    inputOutput.stringType = iter->string;

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type != CLOSE_PARENTHESIS )
                    {
                        ErrorHandle(iter);
                        
                    }

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type == COLUMN )
                    {
                        Forward( iter, { ErrorHandle(iter);  } );

                        if ( iter->type != STRING )
                        {
                            ErrorHandle(iter);
                            
                        }
                        while(iter->type == STRING)
                        {
#ifndef FGDPP_UNIFIED_FGD
                            if(iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH){ ErrorHandle(iter); }
#endif
                            inputOutput.description.push_back(iter->string);
                            Forward( iter, { ErrorHandle(iter); } );
                            if(iter->type == PLUS)
                            {
                                Forward( iter, { ErrorHandle(iter);  } );
                            }
                        }
                    }
                    entity.inputOutput.push_back(inputOutput);
                    continue;
                }
                else
                {
                    //AssignOrResizeArray( entity->entityProperties, EntityProperties_t *, entity->entityPropertyCount, { *err = ErrorFromValues( ALLOCATION_FAILURE, iter->line, iter->range.start, iter->range.end );  } );
                    EntityProperties entityProperties;// = entity->entityProperties[entity->entityPropertyCount - 1] = malloc( sizeof( EntityProperties_t ) );
                    //memset( entityProperties, 0x0, sizeof( EntityProperties_t ) );
                    entityProperties.flagCount = 0;
                    entityProperties.choiceCount = 0;

                    entityProperties.readOnly = false;
                    entityProperties.reportable = false;

//                    entityProperties.propertyName[32] = '\0'; // last character should always be a null terminator.
//                    strncpy( entityProperties->propertyName, iter->string, 31 );
                    entityProperties.propertyName = iter->string;
                    Forward( iter, { ErrorHandle(iter);  } );

#ifdef FGDPP_UNIFIED_FGD
                    if ( iter->type == OPEN_BRACKET )
					{
						Forward( iter, { ErrorHandle(iter);  } );
						if ( !TagListDelimiter( iter, entityProperties.tagList ) )
						{
							ErrorHandle(iter);
							
						}
						Forward( iter, { ErrorHandle(iter);  } );
					}
#endif

                    if ( iter->type != OPEN_PARENTHESIS )
                    {
                        ErrorHandle(iter);
                        
                    }

                    Forward( iter, { ErrorHandle(iter);  } );
                    if ( iter->type != LITERAL )
                    {
                        ErrorHandle(iter);
                        
                    }

                    entityProperties.type = ( iter->string );

                    Forward( iter, { ErrorHandle(iter);  } );
                    if ( iter->type != CLOSE_PARENTHESIS )
                    {
                        ErrorHandle(iter);
                        
                    }

                    Forward( iter, { ErrorHandle(iter);  } );
                    //iequals(iter->string, "readonly")
                    if ( iequals(iter->string, "readonly") )
                    {
                        entityProperties.readOnly = true;
                        Forward( iter, { ErrorHandle(iter);  } );
                    }

                    if ( ( iequals(iter->string, "*") || iequals(iter->string, "report")) )
                    {
                        entityProperties.reportable = true;
                        Forward( iter, { ErrorHandle(iter);  } );
                    }

                    if ( iter->type == EQUALS )
                    {
                        goto isFOC;
                    }

                    if ( iter->type != COLUMN )
                        continue;

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type != STRING )
                    {
                        ErrorHandle(iter);
                        
                    }

                    entityProperties.displayName = ( iter->string );

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type == EQUALS )
                    {
                        goto isFOC;
                    }

                    if ( iter->type != COLUMN )
                        continue;

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type != COLUMN )
                    {
                        entityProperties.defaultValue =( iter->string );
                        Forward( iter, { ErrorHandle(iter);  } );
                    }

                    if ( iter->type == EQUALS )
                    {
                        goto isFOC;
                    }

                    if ( iter->type != COLUMN )
                        continue;

                    Forward( iter, { ErrorHandle(iter);  } );

                    if ( iter->type != STRING )
                    {
                        ErrorHandle(iter);
                        
                    }

                    while(iter->type == STRING)
                    {
#ifndef FGDPP_UNIFIED_FGD
                        if(iter->string.length() > FGDPP_MAX_STR_CHUNK_LENGTH){ ErrorHandle(iter); }
#endif
                        entityProperties.propertyDescription.push_back(iter->string);
                        Forward( iter, { ErrorHandle(iter); } );
                        if(iter->type == PLUS)
                        {
                            Forward( iter, { ErrorHandle(iter);  } );
                        }
                    }

//                    if ( !ProcessFGDStrings( iter, &entityProperties->propertyDescription ) )
//                    {
//                        ErrorHandle(iter);
//
//                    }

                    if ( iter->type == EQUALS )
                    {
                        goto isFOC;
                    }

                    continue;

                    isFOC:
                    {
                        //iequals(entityProperties.type, "flags");
                        bool isFlags = iequals(entityProperties.type, "flags");

                        Forward( iter, { ErrorHandle(iter);  } );

                        if ( iter->type != OPEN_BRACKET )
                        {
                            ErrorHandle(iter);
                            
                        }

                        Forward( iter, { ErrorHandle(iter);  } );

                        while ( iter->type != CLOSE_BRACKET )
                        {
                            if ( isFlags && iter->type != NUMBER )
                            {
                                ErrorHandle(iter);
                                
                            }

                            if ( isFlags )
                            {
                                Flag flags;
                                flags.value = std::stoi( iter->string.data() );

                                Forward( iter, { ErrorHandle(iter);  } );
                                if ( iter->type != COLUMN )
                                {
                                    ErrorHandle(iter);
                                    
                                }

                                Forward( iter, { ErrorHandle(iter);  } );
                                if ( iter->type != STRING )
                                {
                                    ErrorHandle(iter);
                                    
                                }

                                flags.displayName = ( iter->string );

                                if ( std::next( iter )->type == COLUMN )
                                {
                                    Forward( iter, { ErrorHandle(iter);  } );

                                    Forward( iter, { ErrorHandle(iter);  } );
                                    if ( iter->type != NUMBER )
                                    {
                                        ErrorHandle(iter);
                                        
                                    }
                                    flags.checked = iter->string == "1";//strcasecmp( iter->string.data(), "1" ) == 0;

#ifdef FGDPP_UNIFIED_FGD
                                    if ( std::next( iter )->type == OPEN_BRACKET )
								{
									Forward( iter, { ErrorHandle(iter);  } );
									Forward( iter, { ErrorHandle(iter);  } );
									if ( !TagListDelimiter( iter, flags.tagList ) )
									{
										ErrorHandle(iter);
										
									}
								}
#endif
                                }

                                Forward( iter, { ErrorHandle(iter);  } );
                            }
                            else
                            {
                                Choice choice;

                                choice.value = iter->string;

                                Forward( iter, { ErrorHandle(iter);  } );
                                if ( iter->type != COLUMN )
                                {
                                    ErrorHandle(iter);
                                    
                                }

                                Forward( iter, { ErrorHandle(iter);  } );
                                if ( iter->type != STRING )
                                {
                                    ErrorHandle(iter);
                                    
                                }

                                choice.displayName = iter->string;

#ifdef FGDPP_UNIFIED_FGD
                                if ( std::next( iter )->type == OPEN_BRACKET )
							{
								Forward( iter, { ErrorHandle(iter);  } );
								Forward( iter, { ErrorHandle(iter);  } );

								if ( !TagListDelimiter( iter, choice.tagList ) )
								{
									ErrorHandle(iter);
									
								}
							}
#endif

                                Forward( iter, { ErrorHandle(iter);  } );
                            }
                        }
                    }
                }

                Forward( iter, { ErrorHandle(iter);  } );
            }

            this->FGDFileContents.entities.push_back(entity);

        }
    }

    return true;
}


