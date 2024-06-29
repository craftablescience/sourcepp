#include <kvpp/kvpp.h>

#include <BufferStream.h>

#include <sourcepp/parser/Text.h>

using namespace kvpp;
using namespace sourcepp;

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

uint64_t Element::getChildCount() const {
	return this->children.size();
}

uint64_t Element::getChildCount(std::string_view childKey) const {
	uint64_t count = 0;
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
		parser::text::eatWhitespaceAndSingleLineComments(stream);
		if (static_cast<char>(stream.peek(0)) == '}') {
			stream.skip();
			break;
		}
		// Read key
		{
			auto childKey = parser::text::readStringToBuffer(stream, backing, useEscapeSequences);
			elements.push_back({});
			elements.back().key = childKey;
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read value
		if (stream.peek<char>(0) != '{') {
			elements.back().value = parser::text::readStringToBuffer(stream, backing, useEscapeSequences);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read conditional
		if (stream.peek<char>(0) == '[') {
			elements.back().conditional = parser::text::readStringToBuffer(stream, backing, useEscapeSequences, '[', ']');
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read block
		if (stream.peek<char>(0) == '{') {
			stream.skip();
			parser::text::eatWhitespaceAndSingleLineComments(stream);
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
