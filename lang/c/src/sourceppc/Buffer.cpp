#include <sourceppc/Buffer.h>

#include <cstdlib>

#include <sourceppc/Helpers.h>

SOURCEPP_API sourcepp_buffer_t sourcepp_buffer_new(size_t size) {
	sourcepp_buffer_t buffer;
	if (size > 0) {
		buffer.size = static_cast<int64_t>(size);
		buffer.data = static_cast<uint8_t*>(std::malloc(sizeof(uint8_t) * size));
	} else {
		buffer.size = 0;
		buffer.data = nullptr;
	}
	return buffer;
}

SOURCEPP_API void sourcepp_buffer_free(sourcepp_buffer_t* buffer) {
	SOURCEPP_EARLY_RETURN(buffer);

	if (buffer->data) {
		std::free(buffer->data);
		buffer->data = nullptr;
	}
	buffer->size = 0;
}
