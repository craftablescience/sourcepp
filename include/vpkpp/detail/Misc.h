#pragma once

#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

namespace vpkpp::detail {

std::pair<std::string, std::string> splitFilenameAndParentDir(const std::string& filename);

std::vector<std::byte> readFileData(const std::string& filepath, std::size_t preloadBytesOffset = 0);

std::string readFileText(const std::string& filepath, std::size_t preloadBytesOffset = 0);

} // namespace vpkpp::detail
