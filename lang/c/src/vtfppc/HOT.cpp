#include <vtfppc/HOT.h>

#include <vtfpp/HOT.h>

#include <sourceppc/Helpers.h>
#include <vtfppc/Convert.hpp>

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

	return new HOT{std::string{hotPath}};
}

SOURCEPP_API int vtfpp_hot_is_valid(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::hot(handle)->operator bool();
}

SOURCEPP_API uint8_t vtfpp_hot_get_version(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::hot(handle)->getVersion();
}

SOURCEPP_API void vtfpp_hot_set_version(vtfpp_hot_handle_t handle, uint8_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::hot(handle)->setVersion(version);
}

SOURCEPP_API uint8_t vtfpp_hot_get_flags(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::hot(handle)->getFlags();
}

SOURCEPP_API void vtfpp_hot_set_flags(vtfpp_hot_handle_t handle, uint8_t flags) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::hot(handle)->setFlags(flags);
}

SOURCEPP_API uint16_t vtfpp_hot_get_rect_count(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::hot(handle)->getRects().size();
}

SOURCEPP_API vtfpp_hot_rect_t vtfpp_hot_get_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_HOT_RECT_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(Convert::hot(handle)->getRects().size() >= index, VTFPP_HOT_RECT_INVALID);

	const auto& rect = Convert::hot(handle)->getRects().at(index);
	return {
		.flags = rect.flags,
		.x1 = rect.x1,
		.y1 = rect.y1,
		.x2 = rect.x2,
		.y2 = rect.y2,
	};
}

SOURCEPP_API void vtfpp_hot_set_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index, vtfpp_hot_rect_t rect) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(Convert::hot(handle)->getRects().size() >= index);

	auto& rect_ = Convert::hot(handle)->getRects().at(index);
	rect_.flags = static_cast<HOT::Rect::Flags>(rect.flags);
	rect_.x1 = rect.x1;
	rect_.y1 = rect.y1;
	rect_.x2 = rect.x2;
	rect_.y2 = rect.y2;
}

SOURCEPP_API void vtfpp_hot_add_rect(vtfpp_hot_handle_t handle, vtfpp_hot_rect_t rect) {
	SOURCEPP_EARLY_RETURN(handle);

	auto& rect_ = Convert::hot(handle)->getRects().emplace_back();
	rect_.flags = static_cast<HOT::Rect::Flags>(rect.flags);
	rect_.x1 = rect.x1;
	rect_.y1 = rect.y1;
	rect_.x2 = rect.x2;
	rect_.y2 = rect.y2;
}

SOURCEPP_API void vtfpp_hot_remove_rect_at_index(vtfpp_hot_handle_t handle, uint16_t index) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(Convert::hot(handle)->getRects().size() >= index);

	Convert::hot(handle)->getRects().erase(Convert::hot(handle)->getRects().begin() + index);
}

SOURCEPP_API sourcepp_buffer_t vtfpp_hot_bake(vtfpp_hot_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::hot(handle)->bake());
}

SOURCEPP_API int vtfpp_hot_bake_to_file(vtfpp_hot_handle_t handle, const char* hotPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(hotPath, false);

	return Convert::hot(handle)->bake(hotPath);
}

SOURCEPP_API void vtfpp_hot_close(vtfpp_hot_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::hot(*handle);
	*handle = nullptr;
}
