#include <kvpp/kvpp.h>

#include <BufferStream.h>

using namespace kvpp;

namespace {

[[nodiscard]] bool isNewLine(char c) {
	return c == '\n' || c == '\r';
}

[[nodiscard]] bool isWhitespace(char c) {
	return c == ' ' || c == '\a' || c == '\f' || c == '\t' || c == '\v' || ::isNewLine(c);
}

void eatComment(BufferStream& stream) {
	while (!::isNewLine(stream.read<char>())) {}
}

// NOLINTNEXTLINE(*-no-recursion)
void eatWhitespace(BufferStream& stream) {
	while (::isWhitespace(stream.read<char>())) {}
	stream.seek(-1, std::ios::cur);

	if (stream.peek<char>(0) == '/' && (stream.peek<char>(1) == '/' || stream.peek<char>(1) == '*')) {
		stream.skip(2);
		::eatComment(stream);
		::eatWhitespace(stream);
		return;
	}
}

std::string readString(BufferStream& stream, bool useEscapeSequences, char start = '\"', char end = '\"') {
	std::string out;

	bool stopAtWhitespace = true;
	char c = stream.read<char>();
	if (c == start) {
		stopAtWhitespace = false;
	} else {
		out += c;
	}

	for (c = stream.read<char>(); c != end; c = stream.read<char>()) {
		if (stopAtWhitespace && ::isWhitespace(c)) {
			break;
		}
		if (useEscapeSequences && c == '\\') {
			auto n = stream.read<char>();
			if (stopAtWhitespace && ::isWhitespace(n)) {
				break;
			}
			if (n == 'n') {
				out += '\n';
			} else if (n == 't') {
				out += '\t';
			} else if (n == '\\' || n == '\"') {
				out += n;
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

} // namespace

std::string_view Element::getKey() const {
	return this->key;
}

std::string_view Element::getValue() const {
	return this->value;
}

std::string_view Element::getConditional() const {
	return this->conditional;
}

bool Element::hasChild(std::string_view childKey) const {
	return !this->operator[](childKey).isInvalid();
}

unsigned int Element::getChildCount(std::string_view childKey) const {
	unsigned int count = 0;
	for (const Element& element : this->children) {
		if (element.key == childKey) {
			++count;
		}
	}
	return count;
}

const std::vector<Element>& Element::getChildren() const {
	return this->children;
}

const Element& Element::operator[](std::size_t index) const {
	return this->children.at(index);
}

const Element& Element::operator[](std::string_view childKey) const {
	return this->operator()(childKey);
}

const Element& Element::operator()(std::string_view childKey) const {
	for (const Element& element : this->children) {
		if (element.key == childKey) {
			return element;
		}
	}
	return *this;
}

const Element& Element::operator()(std::string_view childKey, unsigned int n) const {
	unsigned int count = 0;
	for (const Element& element : this->children) {
		if (element.key == childKey) {
			if (count == n) {
				return element;
			} else {
				++count;
			}
		}
	}
	return *this;
}

bool Element::isInvalid() const {
	return this == &getInvalid();
}

const Element& Element::getInvalid() {
	static Element element;
	return element;
}

// NOLINTNEXTLINE(*-no-recursion)
void Element::readElementsFrom(BufferStream& stream, std::vector<Element>& elements, bool useEscapeSequences) {
	while (true) {
		::eatWhitespace(stream);
		if (static_cast<char>(stream.peek(0)) == '}') {
			stream.skip();
			break;
		}

		auto childKey = ::readString(stream, useEscapeSequences);
		elements.push_back({});
		auto& element = elements.back();
		element.key = childKey;
		::eatWhitespace(stream);

		if (stream.peek<char>(0) != '{') {
			element.value = ::readString(stream, useEscapeSequences);
			::eatWhitespace(stream);
		}
		if (stream.peek<char>(0) == '[') {
			element.conditional = ::readString(stream, useEscapeSequences, '[', ']');
			::eatWhitespace(stream);
		}
		if (stream.peek<char>(0) == '{') {
			stream.skip();
			::eatWhitespace(stream);
			if (stream.peek<char>(0) != '}') {
				readElementsFrom(stream, element.children, useEscapeSequences);
			} else {
				stream.skip();
			}
		}
	}
}

KV1::KV1(std::string_view kv1Data, bool useEscapeSequences_)
		: Element()
		, useEscapeSequences(useEscapeSequences_) {
	BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};
	try {
		readElementsFrom(stream, this->children, this->useEscapeSequences);
	} catch (const std::overflow_error&) {}
}
