#include <benchmark/benchmark.h>

#include <VTFLib.h>
#include <sourcepp/FS.h>
#include <vtfpp/vtfpp.h>

using namespace sourcepp;
using namespace vtfpp;

// GCC
#pragma GCC optimize("O0")

// Clang
#pragma clang optimize off

// MSVC
#pragma optimize("", off)

namespace {

#define BM_FORMAT(Format, VTFLibFormat) \
	void BM_vtfpp_convert_256x256_##Format##toRGBA8888(benchmark::State& state) { \
		VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/fmt/" #Format ".vtf")}; \
		for ([[maybe_unused]] auto _: state) { \
			auto data = vtf.getImageDataAsRGBA8888(); \
		} \
	} \
	BENCHMARK(BM_vtfpp_convert_256x256_##Format##toRGBA8888); \
	void BM_vtflib_convert_256x256_##Format##toRGBA8888(benchmark::State& state) { \
		VTFLib::CVTFFile vtf; \
		vtf.Load(ASSET_ROOT "vtfpp/fmt/" #Format ".vtf"); \
		for ([[maybe_unused]] auto _: state) { \
			std::vector<std::byte> data(vtf.GetWidth() * vtf.GetHeight() * sizeof(ImagePixel::RGBA8888)); \
			VTFLib::CVTFFile::ConvertToRGBA8888(vtf.GetData(0, 0, 0, 0), reinterpret_cast<vlByte*>(data.data()), vtf.GetWidth(), vtf.GetHeight(), IMAGE_FORMAT_##VTFLibFormat); \
		} \
	} \
	BENCHMARK(BM_vtflib_convert_256x256_##Format##toRGBA8888)

BM_FORMAT(A8,                 A8);
BM_FORMAT(ABGR8888,           ABGR8888);
BM_FORMAT(ARGB8888,           ARGB8888);
BM_FORMAT(BGR565,             BGR565);
BM_FORMAT(BGR888,             BGR888);
BM_FORMAT(BGR888_BLUESCREEN,  BGR888_BLUESCREEN);
BM_FORMAT(BGRA4444,           BGRA4444);
BM_FORMAT(BGRA5551,           BGRA5551);
BM_FORMAT(BGRA8888,           BGRA8888);
BM_FORMAT(BGRX5551,           BGRX5551);
BM_FORMAT(BGRX8888,           BGRX8888);
BM_FORMAT(DXT1,               DXT1);
BM_FORMAT(DXT1_ONE_BIT_ALPHA, DXT1_ONEBITALPHA);
BM_FORMAT(DXT3,               DXT3);
BM_FORMAT(DXT5,               DXT5);
BM_FORMAT(I8,                 I8);
BM_FORMAT(IA88,               IA88);
BM_FORMAT(RGB565,             RGB565);
BM_FORMAT(RGB888,             RGB888);
BM_FORMAT(RGB888_BLUESCREEN,  RGB888_BLUESCREEN);
BM_FORMAT(RGBA8888,           RGBA8888);
BM_FORMAT(UV88,               UV88);
BM_FORMAT(UVLX8888,           UVLX8888);
BM_FORMAT(UVWQ8888,           UVWQ8888);

void BM_vtfpp_createVTF(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		auto data = VTF::create(imageData, ImageFormat::RGBA8888, 2048, 2048, {});
	}
}
BENCHMARK(BM_vtfpp_createVTF);

void BM_vtflib_createVTF(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		VTFLib::CVTFFile vtf;
		vtf.Create(2048, 2048, reinterpret_cast<vlByte*>(imageData.data()), {
			.uiVersion = {7, 4},
			.ImageFormat = IMAGE_FORMAT_DXT1,
			.bMipmaps = true,
			.bThumbnail = true,
			.bReflectivity = true,
		});
		std::vector<std::byte> data(vtf.GetSize());
		vtf.Save(data.data());
	}
}
BENCHMARK(BM_vtflib_createVTF);

} // namespace
