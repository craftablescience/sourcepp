#pragma once

#include <string>

namespace studiomodelpp::internal {

class BufferStream;

void readStringAtOffset(BufferStream& stream, std::string& str);

} // namespace studiomodelpp::internal
