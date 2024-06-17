#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <sourcepp/math/Integer.h>

class BufferStream;

namespace kvpp {

class Element {
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
			return std::stoi(this->value);
		} else if constexpr (std::same_as<T, float>) {
			return std::stof(this->value);
		}
		return T{};
	}

	/// Get the conditional associated with the element
	[[nodiscard]] std::string_view getConditional() const;

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasChild(std::string_view childKey) const;

	/// Get the number of child elements with the given key
	[[nodiscard]] unsigned int getChildCount(std::string_view childKey) const;

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<Element>& getChildren() const;

	/// Get the child element of the element at the given index
	[[nodiscard]] const Element& operator[](std::size_t index) const;

	/// Get the first child element of the element with the given key
	[[nodiscard]] const Element& operator[](std::string_view childKey) const;

	/// Get the first child element of the element with the given key
	[[nodiscard]] const Element& operator()(std::string_view childKey) const;

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const Element& operator()(std::string_view childKey, unsigned int n) const;

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const;

protected:
	std::string key;
	std::string value;
	std::string conditional;
	std::vector<Element> children;

	Element() = default;

	static const Element& getInvalid();

	static void readElementsFrom(BufferStream& stream, std::vector<Element>& element, bool useEscapeSequences);
};

class KV1 : public Element {
public:
	explicit KV1(std::string_view kv1Data, bool useEscapeSequences_ = false);

protected:
	using Element::getKey;
	using Element::getValue;
	using Element::getConditional;

	bool useEscapeSequences;
};

} // namespace kvpp
