#include <kvpp/kvpp.h>

#include <BufferStream.h>

#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

using namespace kvpp;
using namespace sourcepp;

std::string_view KV1Element::getKey() const {
	return this->key;
}

std::string_view KV1Element::getValue() const {
	return this->value;
}

std::string_view KV1Element::getConditional() const {
	return this->conditional;
}

bool KV1Element::hasChild(std::string_view childKey) const {
	return !this->operator[](childKey).isInvalid();
}

uint64_t KV1Element::getChildCount() const {
	return this->children.size();
}

uint64_t KV1Element::getChildCount(std::string_view childKey) const {
	uint64_t count = 0;
	for (const KV1Element& element : this->children) {
		if (string::iequals(element.key, childKey)) {
			++count;
		}
	}
	return count;
}

const std::vector<KV1Element>& KV1Element::getChildren() const {
	return this->children;
}

const KV1Element& KV1Element::operator[](unsigned int n) const {
	return this->children.at(n);
}

const KV1Element& KV1Element::operator[](std::string_view childKey) const {
	return this->operator()(childKey);
}

const KV1Element& KV1Element::operator()(std::string_view childKey) const {
	for (const auto& element : this->children) {
		if (string::iequals(element.key, childKey)) {
			return element;
		}
	}
	return getInvalid();
}

const KV1Element& KV1Element::operator()(std::string_view childKey, unsigned int n) const {
	unsigned int count = 0;
	for (const auto& element : this->children) {
		if (string::iequals(element.key, childKey)) {
			if (count == n) {
				return element;
			} else {
				++count;
			}
		}
	}
	return getInvalid();
}

bool KV1Element::isInvalid() const {
	return this == &getInvalid();
}

const KV1Element& KV1Element::getInvalid() {
	static KV1Element element;
	return element;
}

// NOLINTNEXTLINE(*-no-recursion)
void KV1Element::readElements(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<KV1Element>& elements, const std::unordered_map<char, char>& escapeSequences) {
	while (true) {
		// Check if the block is over
		parser::text::eatWhitespaceAndSingleLineComments(stream);
		if (stream.peek<char>() == '}') {
			stream.skip();
			break;
		}
		// Read key
		{
			auto childKey = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
			elements.push_back({});
			elements.back().key = childKey;
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read value
		if (stream.peek<char>() != '{') {
			elements.back().value = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read conditional
		if (stream.peek<char>() == '[') {
			elements.back().conditional = parser::text::readStringToBuffer(stream, backing, "[", "]", escapeSequences);
			parser::text::eatWhitespaceAndSingleLineComments(stream);
		}
		// Read block
		if (stream.peek<char>() == '{') {
			stream.skip();
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() != '}') {
				readElements(stream, backing, elements.back().children, escapeSequences);
			} else {
				stream.skip();
			}
		}
	}
}

KV1::KV1(std::string_view kv1Data, bool useEscapeSequences_)
		: KV1Element()
		, useEscapeSequences(useEscapeSequences_) {
	if (kv1Data.empty()) {
		return;
	}
	BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};

	// Multiply by 2 to ensure buffer will have enough space (very generous)
	this->backingData.resize(kv1Data.size() * 2);
	BufferStream backing{this->backingData, false};
	try {
		readElements(stream, backing, this->children, this->useEscapeSequences ? parser::text::DEFAULT_ESCAPE_SEQUENCES : parser::text::NO_ESCAPE_SEQUENCES);
	} catch (const std::overflow_error&) {}
}
