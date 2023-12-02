#pragma once

#include <fstream>
#include <string>

namespace studiomodelpp::internal {

class BufferStream;

void readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom = std::ios::cur);

} // namespace studiomodelpp::internal
