#include "benchmark/benchmark.h"

#include "KeyValue.h"
#include "sourcepp/FS.h"
#include "kvpp/kvpp.h"

using namespace kvpp;
using namespace sourcepp;

namespace {

[[nodiscard]] std::string getCustomSizedKeyValues2D(int x, int y) {
	KV1Writer writer;
	for (int i = 0; i < x; i++) {
		auto& level1 = writer.addChild(std::to_string(i));
		for (int j = 0; j < y; j++) {
			auto& level2 = level1.addChild(std::to_string(j));
			level2.setValue(std::to_string(i + j));
		}
	}
	return writer.bake();
}

constexpr int SMALL_DIMS = 16;

[[nodiscard]] std::string_view getKeyValuesSmall() {
	static std::string str = ::getCustomSizedKeyValues2D(SMALL_DIMS, SMALL_DIMS);
	return str;
}

constexpr int LARGE_DIMS = 64;

[[nodiscard]] std::string_view getKeyValuesLarge() {
	static std::string str = ::getCustomSizedKeyValues2D(LARGE_DIMS, LARGE_DIMS);
	return str;
}

void BM_kvpp_read_small(benchmark::State& state) {
	auto kvStr = getKeyValuesSmall();
	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(KV1{kvStr});
	}
}
BENCHMARK(BM_kvpp_read_small);

void BM_speedykeyv_read_small(benchmark::State& state) {
	auto kvStr = getKeyValuesSmall();
	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(KeyValueRoot{kvStr.c_str()});
	}
}
BENCHMARK(BM_speedykeyv_read_small);

void BM_kvpp_read_large(benchmark::State& state) {
	auto kvStr = getKeyValuesLarge();
	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(KV1{kvStr});
	}
}
BENCHMARK(BM_kvpp_read_large);

void BM_speedykeyv_read_large(benchmark::State& state) {
	auto kvStr = getKeyValuesLarge();
	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(KeyValueRoot{kvStr.c_str()});
	}
}
BENCHMARK(BM_speedykeyv_read_large);

void BM_kvpp_write_small(benchmark::State& state) {
	for ([[maybe_unused]] auto _: state) {
		KV1Writer writer;
		for (int i = 0; i < SMALL_DIMS; i++) {
			auto& level1 = writer.addChild(std::to_string(i));
			for (int j = 0; j < SMALL_DIMS; j++) {
				level1[std::to_string(j)] = std::to_string(i + j);
			}
		}
		benchmark::DoNotOptimize(writer.bake());
	}
}
BENCHMARK(BM_kvpp_write_small);

void BM_speedykeyv_write_small(benchmark::State& state) {
	for ([[maybe_unused]] auto _: state) {
		KeyValueRoot root;
		for (int i = 0; i < SMALL_DIMS; i++) {
			auto* level1 = root.AddNode(std::to_string(i).c_str());
			for (int j = 0; j < SMALL_DIMS; j++) {
				level1->Add(std::to_string(j).c_str(), std::to_string(i + j).c_str());
			}
		}
		char* out;
		benchmark::DoNotOptimize((out = root.ToString()));
		delete[] out;
	}
}
BENCHMARK(BM_speedykeyv_write_small);

void BM_kvpp_write_large(benchmark::State& state) {
	for ([[maybe_unused]] auto _: state) {
		KV1Writer writer;
		for (int i = 0; i < LARGE_DIMS; i++) {
			auto& level1 = writer.addChild(std::to_string(i));
			for (int j = 0; j < LARGE_DIMS; j++) {
				level1[std::to_string(j)] = std::to_string(i + j);
			}
		}
		benchmark::DoNotOptimize(writer.bake());
	}
}
BENCHMARK(BM_kvpp_write_large);

void BM_speedykeyv_write_large(benchmark::State& state) {
	for ([[maybe_unused]] auto _: state) {
		KeyValueRoot root;
		for (int i = 0; i < LARGE_DIMS; i++) {
			auto* level1 = root.AddNode(std::to_string(i).c_str());
			for (int j = 0; j < LARGE_DIMS; j++) {
				level1->Add(std::to_string(j).c_str(), std::to_string(i + j).c_str());
			}
		}
		char* out;
		benchmark::DoNotOptimize((out = root.ToString()));
		delete[] out;
	}
}
BENCHMARK(BM_speedykeyv_write_large);

} // namespace
