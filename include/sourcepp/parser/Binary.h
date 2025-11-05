#pragma once

#include <cstddef>
#include <fstream>
#include <string>

#include <sourcepp/Math.h>

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
 * Creates a EightCC identifier from a string of 8 characters.
 * @param eightCC The human-readable FourCC.
 * @return The EightCC identifier.
 */
consteval uint64_t makeEightCC(const char eightCC[8]) {
	return eightCC[0] | (eightCC[1] << 8) | (eightCC[2] << 16) | (eightCC[3] << 24) | (static_cast<uint64_t>(eightCC[4]) << 32) | (static_cast<uint64_t>(eightCC[5]) << 40) | (static_cast<uint64_t>(eightCC[6]) << 48) | (static_cast<uint64_t>(eightCC[7]) << 56);
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
