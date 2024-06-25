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

	if (stream.peek<char>(0) == '/' && stream.peek<char>(1) == '/') {
		stream.skip(2);
		::eatComment(stream);
		::eatWhitespace(stream);
		return;
	}
}

std::string_view readString(BufferStreamReadOnly& stream, BufferStream& backing, bool useEscapeSequences, char start = '\"', char end = '\"') {
	auto startSpan = backing.tell();

	bool stopAtWhitespace = true;
	char c = stream.read<char>();
	if (c == start) {
		stopAtWhitespace = false;
	} else {
		backing << c;
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
				backing << '\n';
			} else if (n == 't') {
				backing << '\t';
			} else if (n == '\\' || n == '\"') {
				backing << n;
			} else {
				backing << c << n;
			}
		} else {
			backing << c;
		}
	}

	backing << '\0';
	return {reinterpret_cast<const char*>(backing.data()) + startSpan, backing.tell() - 1 - startSpan};
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
	return getInvalid();
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
	return getInvalid();
}

bool Element::isInvalid() const {
	return this == &getInvalid();
}

const Element& Element::getInvalid() {
	static Element element;
	return element;
}

// NOLINTNEXTLINE(*-no-recursion)
void Element::readElements(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<Element>& elements, bool useEscapeSequences) {
	while (true) {
		// Check if the block is over
		::eatWhitespace(stream);
		if (static_cast<char>(stream.peek(0)) == '}') {
			stream.skip();
			break;
		}
		// Read key
		{
			auto childKey = ::readString(stream, backing, useEscapeSequences);
			elements.push_back({});
			elements.back().key = childKey;
			::eatWhitespace(stream);
		}
		// Read value
		if (stream.peek<char>(0) != '{') {
			elements.back().value = ::readString(stream, backing, useEscapeSequences);
			::eatWhitespace(stream);
		}
		// Read conditional
		if (stream.peek<char>(0) == '[') {
			elements.back().conditional = ::readString(stream, backing, useEscapeSequences, '[', ']');
			::eatWhitespace(stream);
		}
		// Read block
		if (stream.peek<char>(0) == '{') {
			stream.skip();
			::eatWhitespace(stream);
			if (stream.peek<char>(0) != '}') {
				readElements(stream, backing, elements.back().children, useEscapeSequences);
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
	this->backingData.resize(kv1Data.size());
	BufferStream backing{this->backingData};
	try {
		readElements(stream, backing, this->children, this->useEscapeSequences);
	} catch (const std::overflow_error&) {}
}
