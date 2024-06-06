#pragma once

#include <cstddef>
#include <fstream>
#include <string>

class BufferStream;

namespace sourcepp::buffer {

void readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom = std::ios::cur, std::size_t subtractFromOffset = sizeof(int));

} // namespace sourcepp::buffer
