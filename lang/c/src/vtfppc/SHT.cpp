#include <vtfppc/SHT.h>

#include <vtfpp/SHT.h>

#include <sourceppc/Helpers.h>
#include <vtfppc/Convert.hpp>

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

	return new SHT{std::string{shtPath}};
}

SOURCEPP_API int vtfpp_sht_is_valid(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);

	return Convert::sht(handle)->operator bool();
}

SOURCEPP_API uint32_t vtfpp_sht_get_version(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::sht(handle)->getVersion();
}

SOURCEPP_API void vtfpp_sht_set_version(vtfpp_sht_handle_t handle, uint32_t version) {
	SOURCEPP_EARLY_RETURN(handle);

	Convert::sht(handle)->setVersion(version);
}

SOURCEPP_API uint32_t vtfpp_sht_get_sequences_count(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::sht(handle)->getSequences().size();
}

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_SEQUENCE_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(Convert::sht(handle)->getSequences().size() >= index, VTFPP_SHT_SEQUENCE_INVALID);

	const auto& sequence = Convert::sht(handle)->getSequences().at(index);
	return {
		.id = sequence.id,
		.loop = sequence.loop,
		.frameCount = static_cast<uint32_t>(sequence.frames.size()),
		.durationTotal = sequence.durationTotal,
	};
}

SOURCEPP_API vtfpp_sht_sequence_t vtfpp_sht_get_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_SEQUENCE_INVALID);

	const auto* sequence = Convert::sht(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN_VAL(sequence, VTFPP_SHT_SEQUENCE_INVALID);

	return {
		.id = sequence->id,
		.loop = sequence->loop,
		.frameCount = static_cast<uint32_t>(sequence->frames.size()),
		.durationTotal = sequence->durationTotal,
	};
}

SOURCEPP_API void vtfpp_sht_set_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(Convert::sht(handle)->getSequences().size() >= index);

	auto& sequence_ = Convert::sht(handle)->getSequences().at(index);
	sequence_.id = sequence.id;
	sequence_.loop = sequence.loop;
	if (sequence_.frames.size() != sequence.frameCount) {
		sequence_.frames.resize(sequence.frameCount);
	}
	sequence_.durationTotal = sequence.durationTotal;
}

SOURCEPP_API void vtfpp_sht_set_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence_ = Convert::sht(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN(sequence_);

	sequence_->id = sequence.id;
	sequence_->loop = sequence.loop;
	if (sequence_->frames.size() != sequence.frameCount) {
		sequence_->frames.resize(sequence.frameCount);
	}
	sequence_->durationTotal = sequence.durationTotal;
}

SOURCEPP_API void vtfpp_sht_add_sequence(vtfpp_sht_handle_t handle, vtfpp_sht_sequence_t sequence) {
	SOURCEPP_EARLY_RETURN(handle);

	auto& sequence_ = Convert::sht(handle)->getSequences().emplace_back();
	sequence_.id = sequence.id;
	sequence_.loop = sequence.loop;
	if (sequence_.frames.size() != sequence.frameCount) {
		sequence_.frames.resize(sequence.frameCount);
	}
	sequence_.durationTotal = sequence.durationTotal;
}

SOURCEPP_API void vtfpp_sht_remove_sequence_at_index(vtfpp_sht_handle_t handle, uint32_t index) {
	SOURCEPP_EARLY_RETURN(handle);
	SOURCEPP_EARLY_RETURN(Convert::sht(handle)->getSequences().size() >= index);

	Convert::sht(handle)->getSequences().erase(Convert::sht(handle)->getSequences().begin() + index);
}

SOURCEPP_API void vtfpp_sht_remove_sequence_with_id(vtfpp_sht_handle_t handle, uint32_t id) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence_ = Convert::sht(handle)->getSequenceFromID(id);
	SOURCEPP_EARLY_RETURN(sequence_);

	Convert::sht(handle)->getSequences().erase(Convert::sht(handle)->getSequences().begin() + (sequence_ - Convert::sht(handle)->getSequences().data()));
}

SOURCEPP_API vtfpp_sht_frame_t vtfpp_sht_get_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index) {
	SOURCEPP_EARLY_RETURN_VAL(handle, VTFPP_SHT_FRAME_INVALID);

	auto* sequence = Convert::sht(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN_VAL(sequence, VTFPP_SHT_FRAME_INVALID);
	SOURCEPP_EARLY_RETURN_VAL(sequence->frames.size() >= index, VTFPP_SHT_FRAME_INVALID);

	return {
		.duration = sequence->frames.at(index).duration,
		.bounds = {
			{
				.x1 = sequence->frames.at(index).bounds.at(0).x1,
				.y1 = sequence->frames.at(index).bounds.at(0).y1,
				.x2 = sequence->frames.at(index).bounds.at(0).x2,
				.y2 = sequence->frames.at(index).bounds.at(0).y2,
			},
			{
				.x1 = sequence->frames.at(index).bounds.at(1).x1,
				.y1 = sequence->frames.at(index).bounds.at(1).y1,
				.x2 = sequence->frames.at(index).bounds.at(1).x2,
				.y2 = sequence->frames.at(index).bounds.at(1).y2,
			},
			{
				.x1 = sequence->frames.at(index).bounds.at(2).x1,
				.y1 = sequence->frames.at(index).bounds.at(2).y1,
				.x2 = sequence->frames.at(index).bounds.at(2).x2,
				.y2 = sequence->frames.at(index).bounds.at(2).y2,
			},
			{
				.x1 = sequence->frames.at(index).bounds.at(3).x1,
				.y1 = sequence->frames.at(index).bounds.at(3).y1,
				.x2 = sequence->frames.at(index).bounds.at(3).x2,
				.y2 = sequence->frames.at(index).bounds.at(3).y2,
			},
		}
	};
}

SOURCEPP_API void vtfpp_sht_set_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index, vtfpp_sht_frame_t frame) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = Convert::sht(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);
	SOURCEPP_EARLY_RETURN(sequence->frames.size() >= index);

	auto& frame_ = sequence->frames.at(index);
	frame_.duration = frame.duration;
	frame_.bounds.at(0).x1 = frame.bounds[0].x1;
	frame_.bounds.at(0).y1 = frame.bounds[0].y1;
	frame_.bounds.at(0).x2 = frame.bounds[0].x2;
	frame_.bounds.at(0).y2 = frame.bounds[0].y2;
	frame_.bounds.at(1).x1 = frame.bounds[1].x1;
	frame_.bounds.at(1).y1 = frame.bounds[1].y1;
	frame_.bounds.at(1).x2 = frame.bounds[1].x2;
	frame_.bounds.at(1).y2 = frame.bounds[1].y2;
	frame_.bounds.at(2).x1 = frame.bounds[2].x1;
	frame_.bounds.at(2).y1 = frame.bounds[2].y1;
	frame_.bounds.at(2).x2 = frame.bounds[2].x2;
	frame_.bounds.at(2).y2 = frame.bounds[2].y2;
	frame_.bounds.at(3).x1 = frame.bounds[3].x1;
	frame_.bounds.at(3).y1 = frame.bounds[3].y1;
	frame_.bounds.at(3).x2 = frame.bounds[3].x2;
	frame_.bounds.at(3).y2 = frame.bounds[3].y2;
}

SOURCEPP_API void vtfpp_sht_add_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, vtfpp_sht_frame_t frame) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = Convert::sht(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);

	auto& frame_ = sequence->frames.emplace_back();
	frame_.duration = frame.duration;
	frame_.bounds.at(0).x1 = frame.bounds[0].x1;
	frame_.bounds.at(0).y1 = frame.bounds[0].y1;
	frame_.bounds.at(0).x2 = frame.bounds[0].x2;
	frame_.bounds.at(0).y2 = frame.bounds[0].y2;
	frame_.bounds.at(1).x1 = frame.bounds[1].x1;
	frame_.bounds.at(1).y1 = frame.bounds[1].y1;
	frame_.bounds.at(1).x2 = frame.bounds[1].x2;
	frame_.bounds.at(1).y2 = frame.bounds[1].y2;
	frame_.bounds.at(2).x1 = frame.bounds[2].x1;
	frame_.bounds.at(2).y1 = frame.bounds[2].y1;
	frame_.bounds.at(2).x2 = frame.bounds[2].x2;
	frame_.bounds.at(2).y2 = frame.bounds[2].y2;
	frame_.bounds.at(3).x1 = frame.bounds[3].x1;
	frame_.bounds.at(3).y1 = frame.bounds[3].y1;
	frame_.bounds.at(3).x2 = frame.bounds[3].x2;
	frame_.bounds.at(3).y2 = frame.bounds[3].y2;
}

SOURCEPP_API void vtfpp_sht_remove_frame(vtfpp_sht_handle_t handle, uint32_t sequenceID, uint32_t index) {
	SOURCEPP_EARLY_RETURN(handle);

	auto* sequence = Convert::sht(handle)->getSequenceFromID(sequenceID);
	SOURCEPP_EARLY_RETURN(sequence);
	SOURCEPP_EARLY_RETURN(sequence->frames.size() >= index);

	sequence->frames.erase(sequence->frames.begin() + index);
}

SOURCEPP_API uint8_t vtfpp_sht_get_frame_bounds_count(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, 0);

	return Convert::sht(handle)->getFrameBoundsCount();
}

SOURCEPP_API sourcepp_buffer_t vtfpp_sht_bake(vtfpp_sht_handle_t handle) {
	SOURCEPP_EARLY_RETURN_VAL(handle, SOURCEPP_BUFFER_INVALID);

	return convert::toBuffer(Convert::sht(handle)->bake());
}

SOURCEPP_API int vtfpp_sht_bake_to_file(vtfpp_sht_handle_t handle, const char* shtPath) {
	SOURCEPP_EARLY_RETURN_VAL(handle, false);
	SOURCEPP_EARLY_RETURN_VAL(shtPath, false);

	return Convert::sht(handle)->bake(shtPath);
}

SOURCEPP_API void vtfpp_sht_close(vtfpp_sht_handle_t* handle) {
	SOURCEPP_EARLY_RETURN(handle);

	delete Convert::sht(*handle);
	*handle = nullptr;
}
