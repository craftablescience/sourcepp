#include <kvpp/KV1.h>

using namespace kvpp;
using namespace sourcepp;

KV1::KV1(std::string_view kv1Data, bool useEscapeSequences_)
		: KV1ElementReadable()
		, useEscapeSequences(useEscapeSequences_) {
	if (kv1Data.empty()) {
		return;
	}
	BufferStreamReadOnly stream{kv1Data.data(), kv1Data.size()};

	// Multiply by 2 to ensure buffer will have enough space (very generous)
	this->backingData.resize(kv1Data.size() * 2);
	BufferStream backing{this->backingData, false};
	try {
		read(stream, backing, this->children, parser::text::getDefaultEscapeSequencesOrNone(this->useEscapeSequences));
	} catch (const std::overflow_error&) {}
}
