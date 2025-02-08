#pragma once

#define SOURCEPP_EARLY_RETURN(var) \
	do {                           \
		if (!(var)) {              \
			return;                \
		}                          \
	} while (0)

#define SOURCEPP_EARLY_RETURN_VAL(var, value) \
	do {                                      \
		if (!(var)) {                         \
			return value;                     \
		}                                     \
	} while (0)
