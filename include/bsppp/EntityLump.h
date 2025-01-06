#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <vector>

#include <BufferStream.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/String.h>

namespace bsppp {

template<typename V>
concept BSPEntityKeyValueType = std::convertible_to<V, std::string_view>
                          || std::same_as<V, bool>
                          || std::same_as<V, int32_t>
                          || std::same_as<V, int64_t>
                          || std::same_as<V, float>;

class BSPEntityKeyValues {
public:
	class Element {
		friend class BSPEntityKeyValues;

	public:
		/// Get the key associated with the element
		[[nodiscard]] std::string_view getKey() const;

		/// Set the key associated with the element
		void setKey(std::string_view key_);

		/// Get the value associated with the element
		[[nodiscard]] std::string_view getValue() const;

		/// Get the value associated with the element as the given type
		template<BSPEntityKeyValueType V>
		[[nodiscard]] V getValue() const {
			if constexpr (std::convertible_to<V, std::string_view>) {
				return this->value;
			} else if constexpr (std::same_as<V, bool>) {
				return static_cast<bool>(this->getValue<int32_t>());
			} else if constexpr (std::same_as<V, int32_t>) {
				return std::stoi(std::string{this->value});
			} else if constexpr (std::same_as<V, int64_t>) {
				return std::stoll(std::string{this->value});
			} else if constexpr (std::same_as<V, float>) {
				return std::stof(std::string{this->value});
			}
			return V{};
		}

		/// Set the value associated with the element
		template<BSPEntityKeyValueType V>
		void setValue(V value_) {
			if constexpr (std::convertible_to<V, std::string_view>) {
				this->value = std::string_view{value_};
			} else {
				this->setValue(std::to_string(value_));
			}
		}

		/// Set the value associated with the element
		template<BSPEntityKeyValueType V>
		Element& operator=(V value_) {
			this->setValue(value_);
			return *this;
		}

		/// Check if the given element is invalid
		[[nodiscard]] bool isInvalid() const;

	protected:
		Element() = default;

		static const Element& getInvalid();

		std::string key;
		std::string value;
	};

	BSPEntityKeyValues() = default;

	/// Check if this entity has one or more keyvalues with the given name
	[[nodiscard]] bool hasChild(std::string_view childKey) const;

	/// Get the number of keyvalues
	[[nodiscard]] uint64_t getKeyValuesCount() const;

	/// Get the number of keyvalues with the given key
	[[nodiscard]] uint64_t getKeyValuesCount(std::string_view childKey) const;

	/// Get the keyvalues of the entity
	[[nodiscard]] const std::vector<Element>& getKeyValues() const;

	/// Get the keyvalue of the entity at the given index
	[[nodiscard]] const Element& operator[](unsigned int n) const;

	/// Get the keyvalue of the entity at the given index
	[[nodiscard]] Element& operator[](unsigned int n);

	/// Get the first keyvalue of the entity with the given key
	[[nodiscard]] const Element& operator[](std::string_view childKey) const;

	/// Get the first keyvalue of the entity with the given key, or create a new keyvalue if it doesn't exist
	[[nodiscard]] Element& operator[](std::string_view childKey);

	/// Get the first keyvalue of the entity with the given key
	[[nodiscard]] const Element& operator()(std::string_view childKey) const;

	/// Get the first keyvalue of the entity with the given key, or create a new keyvalue if it doesn't exist
	[[nodiscard]] Element& operator()(std::string_view childKey);

	/// Get the nth keyvalue of the entity with the given key
	[[nodiscard]] const Element& operator()(std::string_view childKey, unsigned int n) const;

	/// Get the nth keyvalue of the element with the given key, or create a new entity if it doesn't exist
	[[nodiscard]] Element& operator()(std::string_view childKey, unsigned int n);

	template<BSPEntityKeyValueType V = std::string_view>
	Element& addKeyValue(std::string_view key_, V value_ = {}) {
		Element elem;
		elem.setKey(key_);
		elem.setValue(value_);
		this->keyvalues.push_back(elem);
		return this->keyvalues.back();
	}

	[[nodiscard]] std::string bake(bool useEscapes) const;

protected:
	std::vector<Element> keyvalues;
};

} // namespace bsppp
