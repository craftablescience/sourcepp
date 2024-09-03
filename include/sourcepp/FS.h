#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

namespace sourcepp::fs {

std::vector<std::byte> readFileBuffer(const std::string& filepath, std::size_t startOffset = 0);

std::string readFileText(const std::string& filepath, std::size_t startOffset = 0);

bool writeFileBuffer(const std::string& filepath, std::span<const std::byte> buffer);

bool writeFileText(const std::string& filepath, const std::string& text);

} // namespace sourcepp::fs
