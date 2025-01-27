#include <bsppp/EntityLump.h>

using namespace bsppp;
using namespace sourcepp;

std::string_view BSPEntityKeyValues::Element::getKey() const {
	return this->key;
}

void BSPEntityKeyValues::Element::setKey(std::string_view key_) {
	this->key = key_;
}

std::string_view BSPEntityKeyValues::Element::getValue() const {
	return this->value;
}

bool BSPEntityKeyValues::Element::isInvalid() const {
	return this == &getInvalid();
}

const BSPEntityKeyValues::Element& BSPEntityKeyValues::Element::getInvalid() {
	static Element element;
	return element;
}

bool BSPEntityKeyValues::hasChild(std::string_view childKey) const {
	return !this->operator[](childKey).isInvalid();
}

uint64_t BSPEntityKeyValues::getKeyValuesCount() const {
	return this->keyvalues.size();
}

uint64_t BSPEntityKeyValues::getKeyValuesCount(std::string_view childKey) const {
	uint64_t count = 0;
	for (const auto& element : this->keyvalues) {
		if (sourcepp::string::iequals(element.getKey(), childKey)) {
			++count;
		}
	}
	return count;
}

const std::vector<BSPEntityKeyValues::Element>& BSPEntityKeyValues::getKeyValues() const {
	return this->keyvalues;
}

const BSPEntityKeyValues::Element& BSPEntityKeyValues::operator[](unsigned int n) const {
	return this->keyvalues.at(n);
}

BSPEntityKeyValues::Element& BSPEntityKeyValues::operator[](unsigned int n) {
	return this->keyvalues.at(n);
}

const BSPEntityKeyValues::Element& BSPEntityKeyValues::operator[](std::string_view childKey) const {
	return this->operator()(childKey);
}

BSPEntityKeyValues::Element& BSPEntityKeyValues::operator[](std::string_view childKey) {
	return this->operator()(childKey);
}

const BSPEntityKeyValues::Element& BSPEntityKeyValues::operator()(std::string_view childKey) const {
	for (const auto& element : this->keyvalues) {
		if (sourcepp::string::iequals(element.getKey(), childKey)) {
			return element;
		}
	}
	return Element::getInvalid();
}

BSPEntityKeyValues::Element& BSPEntityKeyValues::operator()(std::string_view childKey) {
	for (auto& element : this->keyvalues) {
		if (sourcepp::string::iequals(element.getKey(), childKey)) {
			return element;
		}
	}
	return this->addKeyValue(childKey);
}

const BSPEntityKeyValues::Element& BSPEntityKeyValues::operator()(std::string_view childKey, unsigned int n) const {
	unsigned int count = 0;
	for (const auto& element : this->keyvalues) {
		if (sourcepp::string::iequals(element.getKey(), childKey)) {
			if (count == n) {
				return element;
			}
			++count;
		}
	}
	return Element::getInvalid();
}

BSPEntityKeyValues::Element& BSPEntityKeyValues::operator()(std::string_view childKey, unsigned int n) {
	unsigned int count = 0;
	for (auto& element: this->keyvalues) {
		if (sourcepp::string::iequals(element.getKey(), childKey)) {
			if (count == n) {
				return element;
			}
			++count;
		}
	}
	return this->addKeyValue(childKey);
}

void BSPEntityKeyValues::removeKeyValue(std::string_view childKey, int n) {
	unsigned int count = 0;
	for (auto element = this->keyvalues.begin(); element != this->keyvalues.end(); ++element) {
		if (sourcepp::string::iequals(element->getKey(), childKey)) {
			if (n < 0 || count == n) {
				element = this->keyvalues.erase(element);
				if (count == n) {
					return;
				}
			}
			++count;
		}
	}
}

std::string BSPEntityKeyValues::bake(bool useEscapes) const {
	std::string out = "{\n";
	for (const auto& elem : this->keyvalues) {
		out += "\t\"";
		out += parser::text::convertSpecialCharsToEscapes(elem.getKey(), useEscapes ? parser::text::DEFAULT_ESCAPE_SEQUENCES : parser::text::NO_ESCAPE_SEQUENCES);
		out += "\" \"";
		out += parser::text::convertSpecialCharsToEscapes(elem.getValue(), useEscapes ? parser::text::DEFAULT_ESCAPE_SEQUENCES : parser::text::NO_ESCAPE_SEQUENCES);
		out += "\"\n";
	}
	out += "}";
	return out;
}
