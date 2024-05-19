#pragma once

#include <cstddef>
#include <fstream>
#include <string>
#include "BufferStream.h"


namespace sourcepp::detail {

void readStringAtOffset(BufferStream& stream, std::string& str, std::ios::seekdir offsetFrom = std::ios::cur, std::size_t subtractFromOffset = sizeof(int));

template<typename T>
void readValueFromPointerAtOffset(BufferStream &stream, T &str, std::ios_base::seekdir offsetFrom = std::ios::cur, size_t subtractFromOffset = sizeof(int))
{
    int offset = stream.read<int>();

    auto pos = stream.tell();
    stream.seek(offset - subtractFromOffset, offsetFrom);

    auto v = stream.read<T*>();
    if(v == nullptr)
        return;
    str = *stream.read<T*>();
    stream.seek(pos);
}
} // namespace sourcepp::detail
