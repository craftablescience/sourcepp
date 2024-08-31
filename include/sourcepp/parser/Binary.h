#pragma once

#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>

#include <sourcepp/math/Integer.h>

class BufferStream;

namespace sourcepp::parser::binary {

/**
 * Creates a FourCC identifier from a string of 4 characters.
 * @param fourCC The human-readable FourCC.
 * @return The FourCC identifier.
 */
consteval uint32_t makeFourCC(const char fourCC[4]) {
	return fourCC[0] | (fourCC[1] << 8) | (fourCC[2] << 16) | (fourCC[3] << 24);
}

/**
 * Reads an integer from the stream, seeks there, reads a string, and seeks back.
 * @param stream The BufferStream to modify.
 * @param str String contents are read into this.
 * @param offsetFrom The seek direction when reading the string.
 * @param subtractFromOffset This offset is subtracted from the read integer. Defaults to the size of an
 *                           integer since an integer was read from the stream before seeking to the string.
 */
void readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom = std::ios::cur, std::size_t subtractFromOffset = sizeof(int32_t));

} // namespace sourcepp::parser::binary
