#include <vtfppc/HOT.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_create() {
	return new HOT{};
}

SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_mem(const unsigned char* hotData, size_t hotLen) {
	SOURCEPP_EARLY_RETURN_VAL(hotData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(hotLen, nullptr);

	return new HOT{{reinterpret_cast<const std::byte*>(hotData), hotLen}};
}

SOURCEPP_API vtfpp_hot_handle_t vtfpp_hot_open_from_file(const char* hotPath) {
	SOURCEPP_EARLY_RETURN_VAL(hotPath, nullptr);

	return new HOT{hotPath};
}

SOURCEPP_API void vtfpp_hot_close(vtfpp_hot_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<HOT>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_hot_is_valid(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<HOT>(handle)->operator bool();
}

SOURCEPP_API uint8_t vtfpp_hot_get_version(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<HOT>(handle)->getVersion();
}

SOURCEPP_API void vtfpp_hot_set_version(vtfpp_hot_handle_t handle, uint8_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<HOT>(handle)->setVersion(version);
}

SOURCEPP_API uint8_t vtfpp_hot_get_flags(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<HOT>(handle)->getFlags();
}

SOURCEPP_API void vtfpp_hot_set_flags(vtfpp_hot_handle_t handle, uint8_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<HOT>(handle)->setFlags(flags);
}

SOURCEPP_API uint16_t vtfpp_hot_get_rect_count(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<HOT>(handle)->getRects().size();
}

SOURCEPP_API vtfpp_hot_rect_t vtfpp_hot_get_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_HOT_RECT_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(convert::handle<HOT>(handle)->getRects().size() >= index, VTFPP_HOT_RECT_INVALID);

	return convert::cast(convert::handle<HOT>(handle)->getRects().at(index));
}

SOURCEPP_API void vtfpp_hot_set_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index, vtfpp_hot_rect_t rect) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(convert::handle<HOT>(handle)->getRects().size() >= index);

	convert::handle<HOT>(handle)->getRects().at(index) = convert::cast(rect);
}

SOURCEPP_API void vtfpp_hot_add_rect(vtfpp_hot_handle_t handle, vtfpp_hot_rect_t rect) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<HOT>(handle)->getRects().push_back(convert::cast(rect));
}

SOURCEPP_API void vtfpp_hot_remove_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(convert::handle<HOT>(handle)->getRects().size() >= index);

	convert::handle<HOT>(handle)->getRects().erase(convert::handle<HOT>(handle)->getRects().begin() + index);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_hot_bake(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<HOT>(handle)->bake());
}

SOURCEPP_API int vtfpp_hot_bake_to_file(vtfpp_hot_handle_t handle, const char* hotPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(hotPath, false);

	return convert::handle<HOT>(handle)->bake(hotPath);
}
