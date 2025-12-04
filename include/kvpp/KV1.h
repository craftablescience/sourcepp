#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

namespace kvpp {

template<typename V>
concept KV1ValueType = std::convertible_to<V, std::string_view>
                    || std::same_as<V, bool>
                    || std::same_as<V, int32_t>
                    || std::same_as<V, int64_t>
                    || std::same_as<V, float>;

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1ElementReadable {
public:
	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const {
		return this->key;
	}

	/// Get the value associated with the element
	[[nodiscard]] std::string_view getValue() const {
		return this->value;
	}

	/// Get the value associated with the element as the given type
	template<KV1ValueType V>
	[[nodiscard]] V getValue() const {
		if constexpr (std::convertible_to<V, std::string_view>) {
			return this->value;
		} else if constexpr (std::same_as<V, bool>) {
			return static_cast<bool>(this->getValue<int32_t>());
		} else if constexpr (std::same_as<V, int32_t>) {
			if (this->value.length() == 10 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				return std::stoi(std::string{this->value.substr(2)}, nullptr, 16);
			}
			return std::stoi(std::string{this->value});
		} else if constexpr (std::same_as<V, int64_t>) {
			if (this->value.length() == 18 && this->value.starts_with("0x") && sourcepp::parser::text::isNumber(this->value.substr(2))) {
				return std::stoll(std::string{this->value.substr(2)}, nullptr, 16);
			}
			return std::stoll(std::string{this->value});
		} else if constexpr (std::same_as<V, float>) {
			return std::stof(std::string{this->value});
		}
		return V{};
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const {
		return this->conditional;
	}

	/// Check if the element has one or more children with the given key
	[[nodiscard]] bool hasChild(std::string_view childKey) const {
		return !this->operator[](childKey).isInvalid();
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const {
		return this->children.size();
	}

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(std::string_view childKey) const {
		uint64_t count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.key, childKey)) {
				++count;
			}
		}
		return count;
	}

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1ElementReadable>& getChildren() const {
		return this->children;
	}

	using iterator = std::vector<KV1ElementReadable>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = std::vector<KV1ElementReadable>::const_iterator;

	[[nodiscard]] constexpr const_iterator begin() const {
		return this->children.begin();
	}

	[[nodiscard]] constexpr const_iterator end() const {
		return this->children.end();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const {
		return this->children.cbegin();
	}

	[[nodiscard]] constexpr const_iterator cend() const {
		return this->children.cend();
	}

	/// Get the child element of the element at the given index
	[[nodiscard]] const KV1ElementReadable& operator[](unsigned int n) const {
		return this->children.at(n);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator[](std::string_view childKey) const {
		return this->operator()(childKey);
	}

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator()(std::string_view childKey) const {
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				return element;
			}
		}
		return getInvalid();
	}

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1ElementReadable& operator()(std::string_view childKey, unsigned int n) const {
		unsigned int count = 0;
		for (const auto& element : this->children) {
			if (sourcepp::string::iequals(element.getKey(), childKey)) {
				if (count == n) {
					return element;
				}
				if (++count > n) {
					break;
				}
			}
		}
		return getInvalid();
	}

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const {
		return this == &getInvalid();
	}

	static const KV1ElementReadable& getInvalid() {
		static KV1ElementReadable element;
		return element;
	}

	[[nodiscard]] explicit operator bool() const {
		return !this->isInvalid();
	}

protected:
	KV1ElementReadable() = default;

	static void read(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<KV1ElementReadable>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences) {
		using namespace sourcepp;
		while (true) {
			// Check if the block is over
			parser::text::eatWhitespaceAndSingleLineComments(stream);
			if (stream.peek<char>() == '}') {
				stream.skip();
				break;
			}
			// Read key
			{
				elements.push_back(KV1ElementReadable{});
				elements.back().key = parser::text::readStringToBuffer(stream, backing, parser::text::DEFAULT_STRING_START, parser::text::DEFAULT_STRING_END, escapeSequences);
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
					KV1ElementReadable::read(stream, backing, elements.back().children, escapeSequences);
				} else {
					stream.skip();
				}
			}
		}
	}

	S key = ""; // NOLINT(*-redundant-string-init)
	S value = ""; // NOLINT(*-redundant-string-init)
	S conditional = ""; // NOLINT(*-redundant-string-init)
	std::vector<KV1ElementReadable> children;
};

template<typename S = std::string_view>
requires std::convertible_to<S, std::string_view>
class KV1 : public KV1ElementReadable<S> {
public:
	explicit KV1(std::string_view kv1Data, bool useEscapeSequences_ = false)
			: KV1ElementReadable<S>()
			, useEscapeSequences(useEscapeSequences_) {
		if (kv1Data.empty()) {
			return;
		}
		BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};

		// Multiply by 2 to ensure buffer will have enough space (very generous)
		this->backingData.resize(kv1Data.size() * 2);
		BufferStream backing{this->backingData, false};
		try {
			KV1ElementReadable<S>::read(stream, backing, this->children, sourcepp::parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
		} catch (const std::overflow_error&) {}
	}

protected:
	using KV1ElementReadable<S>::getKey;
	using KV1ElementReadable<S>::getValue;
	using KV1ElementReadable<S>::getConditional;

	std::string backingData;
	bool useEscapeSequences;
};

namespace literals {

inline KV1<> operator""_kv1(const char* str, const std::size_t len) {
	return KV1{std::string_view{str, len}};
}

} // namespace literals

} // namespace kvpp
