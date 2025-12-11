#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string_view>
#include <vector>

namespace sourcepp::fs {

std::vector<std::byte> readFileBuffer(const std::filesystem::path& filepath, std::size_t startOffset = 0);

std::string readFileText(const std::filesystem::path& filepath, std::size_t startOffset = 0);

bool writeFileBuffer(const std::filesystem::path& filepath, std::span<const std::byte> buffer);

bool writeFileText(const std::filesystem::path& filepath, std::string_view text);

} // namespace sourcepp::fs
