#include <sourceppc/String.h>

#include <cstdlib>

SOURCEPP_API sourcepp_string_t sourcepp_string_new(size_t size) {
	sourcepp_string_t str;
	if (size > 0) {
		str.size = static_cast<int64_t>(size);
		str.data = static_cast<char*>(std::malloc(sizeof(char) * (size + 1)));
		str.data[size] = '\0';
	} else {
		str.size = 0;
		str.data = nullptr;
	}
	return str;
}

SOURCEPP_API void sourcepp_string_free(sourcepp_string_t* str) {
	if (str->data) {
		std::free(str->data);
		str->data = nullptr;
	}
	str->size = 0;
}

SOURCEPP_API sourcepp_string_array_t sourcepp_string_array_new(size_t size) {
	sourcepp_string_array_t array;
	if (size > 0) {
		array.size = static_cast<int64_t>(size);
		array.data = static_cast<char**>(std::malloc(sizeof(char*) * size));
	} else {
		array.size = 0;
		array.data = nullptr;
	}
	return array;
}

SOURCEPP_API void sourcepp_string_array_free(sourcepp_string_array_t* array) {
	if (array->data) {
		for (size_t i = 0; i < array->size; i++) {
			if (char* str = array->data[i]) {
				std::free(str);
				array->data[i] = nullptr;
			}
		}
		std::free(array->data);
		array->data = nullptr;
	}
	array->size = 0;
}
