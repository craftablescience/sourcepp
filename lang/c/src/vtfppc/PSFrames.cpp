#include <vtfppc/PSFrames.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API vtfpp_psframes_handle_t vtfpp_psframes_open_from_mem(const unsigned char* buffer, size_t bufferLen) {
	SOURCEPP_EARLY_RETURN_VAL(buffer, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(bufferLen, nullptr);

	return new PSFrames{{reinterpret_cast<const std::byte*>(buffer), bufferLen}};
}

SOURCEPP_API vtfpp_psframes_handle_t vtfpp_psframes_open_from_file(const char* psframesPath) {
	SOURCEPP_EARLY_RETURN_VAL(psframesPath, nullptr);

	return new PSFrames{psframesPath};
}

SOURCEPP_API void vtfpp_psframes_close(vtfpp_psframes_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<PSFrames>(*handle);
	*handle = nullptr;
}

SOURCEPP_API uint32_t vtfpp_psframes_get_frame_count(vtfpp_psframes_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PSFrames>(handle)->getFrameCount();
}

SOURCEPP_API uint32_t vtfpp_psframes_get_fps(vtfpp_psframes_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PSFrames>(handle)->getFPS();
}

SOURCEPP_API uint16_t vtfpp_psframes_get_width(vtfpp_psframes_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PSFrames>(handle)->getWidth();
}

SOURCEPP_API uint16_t vtfpp_psframes_get_height(vtfpp_psframes_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<PSFrames>(handle)->getHeight();
}

SOURCEPP_API sourcepp_buffer_t vtfpp_psframes_get_image_data_as(vtfpp_psframes_handle_t handle, vtfpp_image_format_e newFormat, uint32_t frame) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<PSFrames>(handle)->getImageDataAs(convert::cast(newFormat), frame));
}

SOURCEPP_API sourcepp_buffer_t vtfpp_psframes_get_image_data_as_bgr888(vtfpp_psframes_handle_t handle, uint32_t frame) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<PSFrames>(handle)->getImageDataAsBGR888(frame));
}
