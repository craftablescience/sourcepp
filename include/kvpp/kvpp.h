#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <sourcepp/math/Integer.h>

class BufferStream;
class BufferStreamReadOnly;

namespace kvpp {

class KV1Element {
public:
	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const;

	/// Get the value associated with the element
	[[nodiscard]] std::string_view getValue() const;

	/// Get the value associated with the element as the given type
	template<typename T>
	requires std::same_as<T, std::string_view> || std::same_as<T, bool> || std::same_as<T, int> || std::same_as<T, float>
	[[nodiscard]] T getValue() const {
		if constexpr (std::same_as<T, std::string_view>) {
			return this->value;
		} else if constexpr (std::same_as<T, bool>) {
			return static_cast<bool>(this->getValue<int>());
		} else if constexpr (std::same_as<T, int>) {
			return std::stoi(std::string{this->value});
		} else if constexpr (std::same_as<T, float>) {
			return std::stof(std::string{this->value});
		}
		return T{};
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const;

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasChild(std::string_view childKey) const;

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const;

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(std::string_view childKey) const;

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1Element>& getChildren() const;

	/// Get the child element of the element at the given index
	[[nodiscard]] const KV1Element& operator[](unsigned int n) const;

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1Element& operator[](std::string_view childKey) const;

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1Element& operator()(std::string_view childKey) const;

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1Element& operator()(std::string_view childKey, unsigned int n) const;

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const;

protected:
	std::string_view key;
	std::string_view value = ""; // NOLINT(*-redundant-string-init)
	std::string_view conditional = ""; // NOLINT(*-redundant-string-init)
	std::vector<KV1Element> children;

	KV1Element() = default;

	static const KV1Element& getInvalid();

	static void readElements(BufferStreamReadOnly& stream, BufferStream& backing, std::vector<KV1Element>& element, const std::unordered_map<char, char>& escapeSequences);
};

class KV1 : public KV1Element {
public:
	explicit KV1(std::string_view kv1Data, bool useEscapeSequences_ = false);

protected:
	using KV1Element::getKey;
	using KV1Element::getValue;
	using KV1Element::getConditional;

	std::string backingData;
	bool useEscapeSequences;
};

} // namespace kvpp
