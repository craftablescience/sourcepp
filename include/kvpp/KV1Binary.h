#pragma once

#include <concepts>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>

namespace kvpp {

enum class KV1BinaryValueType : uint8_t {
	CHILDREN,
	STRING,
	INT32,
	FLOAT,
	POINTER,
	WSTRING,
	COLOR_RGBA,
	UINT64,
	COUNT,
};

using KV1BinaryPointer = uint32_t;

using KV1BinaryValue = std::variant<
	std::monostate,
	std::string,
	int32_t,
	float,
	KV1BinaryPointer,
	std::wstring,
	sourcepp::math::Vec4ui8,
	uint64_t
>;

template<typename V>
concept KV1BinaryValueNoChildren = std::same_as<V, std::string>
                                || std::same_as<V, int32_t>
                                || std::same_as<V, float>
                                || std::same_as<V, KV1BinaryPointer>
                                || std::same_as<V, std::wstring>
                                || std::same_as<V, sourcepp::math::Vec4ui8>
                                || std::same_as<V, uint64_t>;

class KV1BinaryElement {
public:
	KV1BinaryElement() = default;

	/// Get the key associated with the element
	[[nodiscard]] std::string_view getKey() const;

	/// Set the key associated with the element
	void setKey(std::string_view key_);

	/// Get the value associated with the element
	[[nodiscard]] const KV1BinaryValue& getValue() const;

	/// Get the value associated with the element as the given type
	template<KV1BinaryValueNoChildren V>
	[[nodiscard]] std::optional<V> getValue() const {
		if (!std::holds_alternative<V>(this->value)) {
			return std::nullopt;
		}
		return std::get<V>(this->value);
	}

	/// Set the value associated with the element
	void setValue(KV1BinaryValue value_);

	/// Set the value associated with the element
	KV1BinaryElement& operator=(KV1BinaryValue value_);

	/// Set the value associated with the element
	template<KV1BinaryValueNoChildren V>
	void setValue(V value_) {
		this->value = std::move(value_);
	}

	/// Set the value associated with the element
	template<KV1BinaryValueNoChildren V>
	KV1BinaryElement& operator=(V value_) {
		this->setValue(std::move(value_));
		return *this;
	}

	/// Check if the element has one or more children with the given name
	[[nodiscard]] bool hasChild(std::string_view childKey) const;

	/// Add a child element to the element
	KV1BinaryElement& addChild(std::string_view key_, KV1BinaryValue value_ = {});

	/// Add a child element to the element
	template<KV1BinaryValueNoChildren V>
	KV1BinaryElement& addChild(std::string_view key_, V value_ = {}) {
		KV1BinaryElement elem;
		elem.setKey(key_);
		elem.setValue(std::move(value_));
		this->children.push_back(elem);
		return this->children.back();
	}

	/// Get the number of child elements
	[[nodiscard]] uint64_t getChildCount() const;

	/// Get the number of child elements with the given key
	[[nodiscard]] uint64_t getChildCount(std::string_view childKey) const;

	/// Get the child elements of the element
	[[nodiscard]] const std::vector<KV1BinaryElement>& getChildren() const;

	using iterator = std::vector<KV1BinaryElement>::iterator;

	[[nodiscard]] constexpr iterator begin() {
		return this->children.begin();
	}

	[[nodiscard]] constexpr iterator end() {
		return this->children.end();
	}

	using const_iterator = std::vector<KV1BinaryElement>::const_iterator;

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
	[[nodiscard]] const KV1BinaryElement& operator[](unsigned int n) const;

	/// Get the child element of the element at the given index
	[[nodiscard]] KV1BinaryElement& operator[](unsigned int n);

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1BinaryElement& operator[](std::string_view childKey) const;

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1BinaryElement& operator[](std::string_view childKey);

	/// Get the first child element of the element with the given key
	[[nodiscard]] const KV1BinaryElement& operator()(std::string_view childKey) const;

	/// Get the first child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1BinaryElement& operator()(std::string_view childKey);

	/// Get the nth child element of the element with the given key
	[[nodiscard]] const KV1BinaryElement& operator()(std::string_view childKey, unsigned int n) const;

	/// Get the nth child element of the element with the given key, or create a new element if it doesn't exist
	[[nodiscard]] KV1BinaryElement& operator()(std::string_view childKey, unsigned int n);

	/// Remove a child element from the element.
	void removeChild(unsigned int n);

	/// Remove a child element from the element. -1 means all children with the given key
	void removeChild(std::string_view childKey, int n = -1);

	/// Check if the given element is invalid
	[[nodiscard]] bool isInvalid() const;

	static const KV1BinaryElement& getInvalid();

	[[nodiscard]] explicit operator bool() const;

protected:
	// NOLINTNEXTLINE(*-no-recursion)
	static void read(BufferStreamReadOnly& stream, std::vector<KV1BinaryElement>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences);

	static void write(BufferStream& stream, const std::vector<KV1BinaryElement>& elements, const sourcepp::parser::text::EscapeSequenceMap& escapeSequences);

	std::string key;
	KV1BinaryValue value;
	std::vector<KV1BinaryElement> children;
};

class KV1Binary : public KV1BinaryElement {
public:
	explicit KV1Binary(std::span<const std::byte> kv1Data = {}, bool useEscapeSequences_ = false);

	[[nodiscard]] std::vector<std::byte> bake() const;

	void bake(const std::string& kv1Path) const;

	[[nodiscard]] std::string bakeText() const;

	void bakeText(const std::string& kv1Path) const;

protected:
	bool useEscapeSequences;
};

} // namespace kvpp
