#include <vtfppc/SHT.h>

#include <sourceppc/Helpers.h>

using namespace sourceppc;
using namespace vtfpp;

SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_create() {
	return new SHT{};
}

SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_mem(const unsigned char* shtData, size_t shtLen) {
	SOURCEPP_EARLY_RETURN_VAL(shtData, nullptr);
	SOURCEPP_EARLY_RETURN_VAL(shtLen, nullptr);

	return new SHT{{reinterpret_cast<const std::byte*>(shtData), shtLen}};
}

SOURCEPP_API vtfpp_sht_handle_t vtfpp_sht_open_from_file(const char* shtPath) {
	SOURCEPP_EARLY_RETURN_VAL(shtPath, nullptr);

	return new SHT{shtPath};
}

SOURCEPP_API void vtfpp_sht_free(vtfpp_sht_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete convert::handle<SHT>(*handle);
	*handle = nullptr;
}

SOURCEPP_API int vtfpp_sht_is_valid(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return convert::handle<SHT>(handle)->operator bool();
}

SOURCEPP_API uint32_t vtfpp_sht_get_version(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<SHT>(handle)->getVersion();
}

SOURCEPP_API void vtfpp_sht_set_version(vtfpp_sht_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	convert::handle<SHT>(handle)->setVersion(version);
}

SOURCEPP_API uint32_t vtfpp_sht_get_sequences_count(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<SHT>(handle)->getSequences().size();
}

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_SEQUENCE_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(convert::handle<SHT>(handle)->getSequences().size() >= index, VTFPP_SHT_SEQUENCE_INVALID);

	return convert::cast(convert::handle<SHT>(handle)->getSequences().at(index));
}

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_SEQUENCE_INVALID);

	const auto* sequence = convert::handle<SHT>(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN_VAL(sequence, VTFPP_SHT_SEQUENCE_INVALID);

	return convert::cast(*sequence);
}

SOURCEPP_API void vtfpp_sht_set_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(convert::handle<SHT>(handle)->getSequences().size() >= index);

	convert::set(convert::handle<SHT>(handle)->getSequences().at(index), sequence);
}

SOURCEPP_API void vtfpp_sht_set_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence_ = convert::handle<SHT>(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN(sequence_);

	convert::set(*sequence_, sequence);
}

SOURCEPP_API void vtfpp_sht_add_sequence(vtfpp_sht_handle_t handle, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);

	auto& sequence_ = convert::handle<SHT>(handle)->getSequences().emplace_back();
	convert::set(sequence_, sequence);
}

SOURCEPP_API void vtfpp_sht_remove_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(convert::handle<SHT>(handle)->getSequences().size() >= index);

	convert::handle<SHT>(handle)->getSequences().erase(convert::handle<SHT>(handle)->getSequences().begin() + index);
}

SOURCEPP_API void vtfpp_sht_remove_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id) {
	SOURCEPP_EARLY_RETURN(handle);

	const auto* sequence_ = convert::handle<SHT>(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN(sequence_);

	convert::handle<SHT>(handle)->getSequences().erase(convert::handle<SHT>(handle)->getSequences().begin() + (sequence_ - convert::handle<SHT>(handle)->getSequences().data()));
}

SOURCEPP_API vtfpp_sht_sequence_frame_t vtfpp_sht_get_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_SEQUENCE_FRAME_INVALID);

	const auto* sequence = convert::handle<SHT>(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN_VAL(sequence, VTFPP_SHT_SEQUENCE_FRAME_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(sequence->frames.size() >= index, VTFPP_SHT_SEQUENCE_FRAME_INVALID);

	return convert::cast(sequence->frames.at(index));
}

// ReSharper disable once CppPassValueParameterByConstReference
SOURCEPP_API void vtfpp_sht_set_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index, vtfpp_sht_sequence_frame_t frame) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = convert::handle<SHT>(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);
	SOURCEPP_EARLY_RETURN(sequence->frames.size() >= index);

	sequence->frames.at(index) = convert::cast(frame);
}

// ReSharper disable once CppPassValueParameterByConstReference
SOURCEPP_API void vtfpp_sht_add_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, vtfpp_sht_sequence_frame_t frame) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = convert::handle<SHT>(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);

	sequence->frames.push_back(convert::cast(frame));
}

SOURCEPP_API void vtfpp_sht_remove_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = convert::handle<SHT>(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);
	SOURCEPP_EARLY_RETURN(sequence->frames.size() >= index);

	sequence->frames.erase(sequence->frames.begin() + index);
}

SOURCEPP_API uint8_t vtfpp_sht_get_frame_bounds_count(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return convert::handle<SHT>(handle)->getFrameBoundsCount();
}

SOURCEPP_API sourcepp_buffer_t vtfpp_sht_bake(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(convert::handle<SHT>(handle)->bake());
}

SOURCEPP_API int vtfpp_sht_bake_to_file(vtfpp_sht_handle_t handle, const char* shtPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(shtPath, false);

	return convert::handle<SHT>(handle)->bake(shtPath);
}
