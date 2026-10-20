#include <sourcepp/parser/Text.h>

#include <algorithm>
#include <cctype>

#include <BufferStream.h>

using namespace sourcepp;

const parser::text::EscapeSequenceMap& parser::text::getDefaultEscapeSequences() {
	static const EscapeSequenceMap defaultEscapeSequences{
			{'\'', '\''},
			{'\"', '\"'},
			{'?',   '?'},
			{'\\', '\\'},
			{'a',  '\a'},
			{'b',  '\b'},
			{'f',  '\f'},
			{'n',  '\n'},
			{'r',  '\r'},
			{'t',  '\t'},
			{'v',  '\v'},
	};
	return defaultEscapeSequences;
}

const parser::text::EscapeSequenceMap& parser::text::getDefaultEscapeSequencesOrNone(bool useEscapes) {
	static const EscapeSequenceMap noEscapeSequences{};
	return useEscapes ? getDefaultEscapeSequences() : noEscapeSequences;
}

const std::vector<std::string_view>& parser::text::Tokenizer::Result::getTokens() const {
	return this->tokens;
}

uint64_t parser::text::Tokenizer::Result::size() const {
	return this->tokens.size();
}

uint64_t parser::text::Tokenizer::Result::tell() const {
	return this->head;
}

void parser::text::Tokenizer::Result::seek(uint64_t head_) {
	this->head = std::clamp<uint64_t>(head_, 0, this->tokens.size());
}

std::string_view parser::text::Tokenizer::Result::next() {
	if (this->head >= this->tokens.size()) {
		throw std::overflow_error{""};
	}
	return this->tokens[this->head++];
}

bool parser::text::Tokenizer::Result::nextBool(bool& value) {
	return string::toBool(this->next(), value);
}

std::string_view parser::text::Tokenizer::Result::peek() const {
	if (this->head >= this->tokens.size()) {
		throw std::overflow_error{""};
	}
	return this->tokens[this->head];
}

bool parser::text::Tokenizer::Result::peekBool(bool& value) const {
	return string::toBool(this->peek(), value);
}

void parser::text::Tokenizer::Result::skip() {
	this->head++;
}

parser::text::Tokenizer& parser::text::Tokenizer::setSingleLineCommentStart(std::string_view singleLineCommentStart_) {
	this->singleLineCommentStart = singleLineCommentStart_;
	return *this;
}

parser::text::Tokenizer& parser::text::Tokenizer::setMultiLineCommentStart(std::string_view multiLineCommentStart_) {
	this->multiLineCommentStart = multiLineCommentStart_;
	return *this;
}

parser::text::Tokenizer& parser::text::Tokenizer::setMultiLineCommentEnd(std::string_view multiLineCommentEnd_) {
	this->multiLineCommentEnd = multiLineCommentEnd_;
	return *this;
}

//parser::text::Tokenizer& parser::text::Tokenizer::setQuotes(std::initializer_list<std::pair<char, char>> quotes_) {
//	this->quotes = quotes_;
//	return *this;
//}

parser::text::Tokenizer& parser::text::Tokenizer::setEscapeSequences(const EscapeSequenceMap& escapeSequences_) {
	this->escapeSequences = escapeSequences_;
	return *this;
}

parser::text::Tokenizer::Result parser::text::Tokenizer::parse(std::string_view text, std::string& backingString) const {
	BufferStreamReadOnly stream{text};

	backingString.resize(text.length());
	BufferStream backing{backingString};

	Result result;
	try {
		while (stream.tell() < stream.size()) {
			if (this->singleLineCommentStart.empty()) {
				if (this->multiLineCommentStart.empty()) {
					eatWhitespace(stream);
				} else {
					eatWhitespaceAndMultiLineComments(stream);
				}
			} else {
				if (this->multiLineCommentStart.empty()) {
					eatWhitespaceAndSingleLineComments(stream);
				} else {
					eatWhitespaceAndComments(stream);
				}
			}
			result.tokens.push_back(readStringToBuffer(stream, backing, /*this->quotes*/ {DEFAULT_QUOTES}, this->escapeSequences));
		}
	} catch (const std::overflow_error&) {}
	return result;
}

bool parser::text::isNewLine(char c) {
	return c == '\n' || c == '\r';
}

bool parser::text::isNewLine(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isNewLine(c); });
}

bool parser::text::isWhitespace(char c) {
	return c == ' ' || c == '\a' || c == '\f' || c == '\t' || c == '\v' || isNewLine(c);
}

bool parser::text::isWhitespace(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isWhitespace(c); });
}

bool parser::text::isNumber(char c) {
	return std::isdigit(c);
}

bool parser::text::isNumber(std::string_view str) {
	return std::ranges::all_of(str, [](char c) { return isNumber(c); });
}

std::string parser::text::convertSpecialCharsToEscapes(std::string_view str, const EscapeSequenceMap& escapeSequences) {
	// Reverse escape sequences map (assume that it's bidirectional)
	EscapeSequenceMap specialSequences;
	for (const auto& [normal, special] : escapeSequences) {
		specialSequences[special] = normal;
	}

	std::string out;
	for (int i = 0; i < str.length(); i++) {
		if (specialSequences.contains(str[i])) {
			out += '\\';
			out += specialSequences[str[i]];
		} else {
			out += str[i];
		}
	}
	return out;
}

std::string parser::text::convertEscapesToSpecialChars(std::string_view str, const EscapeSequenceMap& escapeSequences) {
	std::string out;
	for (int i = 0; i < str.length(); i++) {
		if (!escapeSequences.empty() && str[i] == '\\' && i < str.length() - 1) {
			if (auto n = str[i + 1]; escapeSequences.contains(n)) {
				out += escapeSequences.at(n);
			} else {
				out += str[i];
				out += str[i + 1];
			}
		} else {
			out += str[i];
		}
	}
	return out;
}

void parser::text::eatWhitespace(BufferStream& stream) {
	while (stream.tell() < stream.size() && isWhitespace(stream.read<char>())) {}
	if (stream.tell() == stream.size() && isWhitespace(stream.seek(-1, std::ios::cur).read<char>())) return;
	stream.seek(-1, std::ios::cur);
}

void parser::text::eatSingleLineComment(BufferStream& stream) {
	while (stream.tell() < stream.size() && !isNewLine(stream.read<char>())) {}
}

void parser::text::eatMultiLineComment(BufferStream& stream, std::string_view multiLineCommentEnd) {
	while (!std::ranges::equal(stream.read_span<char>(multiLineCommentEnd.length()), multiLineCommentEnd)) {
		stream.seek(-static_cast<int64_t>(multiLineCommentEnd.length() - 1), std::ios::cur);
	}
}

void parser::text::eatWhitespaceAndSingleLineComments(BufferStream& stream, std::string_view singleLineCommentStart) {
	return eatWhitespaceAndComments(stream, singleLineCommentStart, "");
}

void parser::text::eatWhitespaceAndMultiLineComments(BufferStream& stream, std::string_view multiLineCommentStart) {
	return eatWhitespaceAndComments(stream, "", multiLineCommentStart);
}

// NOLINTNEXTLINE(*-no-recursion)
void parser::text::eatWhitespaceAndComments(BufferStream& stream, std::string_view singleLineCommentStart, std::string_view multiLineCommentStart) {
	eatWhitespace(stream);

	if (!singleLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(singleLineCommentStart.length()), singleLineCommentStart)) {
			eatSingleLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		}
		stream.seek(-static_cast<int64_t>(singleLineCommentStart.length()), std::ios::cur);
	}

	if (!multiLineCommentStart.empty()) {
		if (std::ranges::equal(stream.read_span<char>(multiLineCommentStart.length()), multiLineCommentStart)) {
			eatMultiLineComment(stream);
			eatWhitespaceAndComments(stream, singleLineCommentStart, multiLineCommentStart);
			return;
		}
		stream.seek(-static_cast<int64_t>(multiLineCommentStart.length()), std::ios::cur);
	}
}

bool parser::text::tryToEatChar(BufferStream& stream, char c) {
	if (stream.tell() == stream.size() || stream.peek<char>() != c) {
		return false;
	}
	stream.skip();
	return true;
}

std::string parser::text::readString(BufferStream& stream, std::initializer_list<std::pair<char, char>> quotes, const EscapeSequenceMap& escapeSequences) {
	std::string out;

	bool stopAtWhitespace = true;
	std::pair<char, char> quoteUsed;
	auto c = stream.read<char>();
	for (const auto& quote : quotes) {
		if (c == quote.first) {
			stopAtWhitespace = false;
			quoteUsed = quote;
			break;
		}
	}
	if (stopAtWhitespace) {
		out += c;
	}

	for (c = stream.read<char>(); (stopAtWhitespace && !isWhitespace(c)) || (!stopAtWhitespace && quoteUsed.second != c); stream >> c) {
		if (!escapeSequences.empty() && c == '\\') {
			auto n = stream.read<char>();
			if (stopAtWhitespace && isWhitespace(n)) {
				break;
			}
			if (escapeSequences.contains(n)) {
				out += escapeSequences.at(n);
			} else if (!stopAtWhitespace && quoteUsed.second == c) {
				break;
			} else {
				out += c;
				out += n;
			}
		} else {
			out += c;
		}
	}

	return out;
}

std::string_view parser::text::readStringToBuffer(BufferStream& stream, BufferStream& backing, std::initializer_list<std::pair<char, char>> quotes, const EscapeSequenceMap& escapeSequences) {
	const auto startSpan = backing.tell();

	bool stopAtWhitespace = true;
	std::pair<char, char> quoteUsed;
	auto c = stream.read<char>();
	for (const auto& quote : quotes) {
		if (c == quote.first) {
			stopAtWhitespace = false;
			quoteUsed = quote;
			break;
		}
	}
	if (stopAtWhitespace) {
		backing << c;
	}

	for (c = stream.read<char>(); (stopAtWhitespace && !isWhitespace(c)) || (!stopAtWhitespace && quoteUsed.second != c); stream >> c) {
		if (!escapeSequences.empty() && c == '\\') {
			auto n = stream.read<char>();
			if (stopAtWhitespace && isWhitespace(n)) {
				break;
			}
			if (escapeSequences.contains(n)) {
				backing << escapeSequences.at(n);
			} else if (!stopAtWhitespace && quoteUsed.second == c) {
				break;
			} else {
				backing << c << n;
			}
		} else {
			backing << c;
		}
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, static_cast<std::string_view::size_type>(backing.tell() - 1 - startSpan)};
}

std::string parser::text::readUnquotedString(BufferStream& stream, const EscapeSequenceMap& escapeSequences) {
	return readString(stream, {}, escapeSequences);
}

std::string_view parser::text::readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, const EscapeSequenceMap& escapeSequences) {
	return readStringToBuffer(stream, backing, {}, escapeSequences);
}

std::string parser::text::readUnquotedString(BufferStream& stream, std::string_view end, const EscapeSequenceMap& escapeSequences) {
	std::string out;

	for (char c = stream.read<char>(); !isWhitespace(c) && end.find(c) == std::string_view::npos; c = stream.read<char>()) {
		if (!escapeSequences.empty() && c == '\\') {
			if (auto n = stream.read<char>(); escapeSequences.contains(n)) {
				out += escapeSequences.at(n);
			} else if (isWhitespace(n) || end.find(n) != std::string_view::npos) {
				break;
			} else {
				out += c;
				out += n;
			}
		} else {
			out += c;
		}
	}

	return out;
}

std::string_view parser::text::readUnquotedStringToBuffer(BufferStream& stream, BufferStream& backing, std::string_view end, const EscapeSequenceMap& escapeSequences) {
	const auto startSpan = backing.tell();

	for (char c = stream.read<char>(); !isWhitespace(c) && end.find(c) == std::string_view::npos; c = stream.read<char>()) {
		if (!escapeSequences.empty() && c == '\\') {
			if (auto n = stream.read<char>(); escapeSequences.contains(n)) {
				backing << escapeSequences.at(n);
			} else if (isWhitespace(n) || end.find(n) != std::string_view::npos) {
				break;
			} else {
				backing << c << n;
			}
		} else {
			backing << c;
		}
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, static_cast<std::string_view::size_type>(backing.tell() - 1 - startSpan)};
}
