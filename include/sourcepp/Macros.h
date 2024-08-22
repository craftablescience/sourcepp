#pragma once

#define SOURCEPP_CONCAT_INNER(a, b) a##b
#define SOURCEPP_CONCAT(a, b) SOURCEPP_CONCAT_INNER(a, b)

/// Adds the current line number to the given base
#define SOURCEPP_UNIQUE_NAME(base) SOURCEPP_CONCAT(base, __LINE__)
