#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <type_traits>
#include <vector>

namespace studiomodelpp::internal {

class BufferStream {
public:
    BufferStream(const std::byte* buffer, std::uint64_t bufferLength);

    BufferStream(const BufferStream& other) = delete;
    BufferStream& operator=(const BufferStream& other) = delete;

    BufferStream(BufferStream&& other) noexcept = default;
    BufferStream& operator=(BufferStream&& other) noexcept = default;

    void seek(std::uint64_t offset, std::ios::seekdir offsetFrom = std::ios::beg);

    void skip(std::uint64_t offset);

	template<typename T, std::size_t N = 1, std::enable_if_t<std::is_trivially_copyable_v<T> && std::is_trivially_constructible_v<T>, bool> = true>
	void skip() {
		this->skip(sizeof(T) * N);
	}

    [[nodiscard]] std::uint64_t tell() const;

    template<std::uint64_t L>
    [[nodiscard]] std::array<std::byte, L> readBytes() {
        std::array<std::byte, L> out;
        for (int i = 0; i < L; i++, this->streamPos++) {
			out[i] = this->streamBuffer[this->streamPos];
		}
        return out;
    }

    [[nodiscard]] std::vector<std::byte> readBytes(std::uint64_t length);

    template<typename T, std::enable_if_t<std::is_trivially_copyable_v<T> && std::is_trivially_constructible_v<T>, bool> = true>
    [[nodiscard]] T read() {
        T obj{};
        this->read(obj);
        return obj;
    }

    template<typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, bool> = true>
    void read(T& obj) {
        for (int i = 0; i < sizeof(T); i++, this->streamPos++) {
			reinterpret_cast<std::byte*>(&obj)[i] = this->streamBuffer[this->streamPos];
		}
    }

	template<typename T, std::size_t N, std::enable_if_t<std::is_trivially_copyable_v<T>, bool> = true>
	void read(T(&obj)[N]) {
		for (int i = 0; i < sizeof(T) * N; i++, this->streamPos++) {
			reinterpret_cast<std::byte*>(&obj)[i] = this->streamBuffer[this->streamPos];
		}
	}

	template<typename T, std::size_t N>
	void read(std::array<T, N>& obj) {
		for (int i = 0; i < N; i++) {
			obj[i] = this->read<T>();
		}
	}

	template<typename T>
	void read(std::vector<T>& obj, std::size_t n) {
		obj.clear();
		obj.reserve(n);
		for (int i = 0; i < n; i++) {
			obj.push_back(this->read<T>());
		}
	}

    void read(std::string& obj) {
        char temp = this->read<char>();
        while (temp != '\0') {
            obj += temp;
            temp = this->read<char>();
        }
    }

    [[nodiscard]] std::byte peek(long offset = 0);

protected:
    const std::byte* streamBuffer;
    std::uint64_t streamLen;
    std::uint64_t streamPos;
};

} // namespace studiomodelpp::detail
