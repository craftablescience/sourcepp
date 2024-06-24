#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace sourcepp::fs {

std::vector<std::byte> readFileBuffer(const std::string& filepath, std::size_t startOffset = 0);

std::string readFileText(const std::string& filepath, std::size_t startOffset = 0);

} // namespace sourcepp::fs
