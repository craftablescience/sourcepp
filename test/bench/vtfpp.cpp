#include <benchmark/benchmark.h>

#include <VTFLib.h>
#include <sourcepp/FS.h>
#include <vtfpp/vtfpp.h>

using namespace sourcepp;
using namespace vtfpp;

namespace {

#define BM_FORMAT(Format, VTFLibFormat) \
	void BM_vtfpp_convert_256x256_##Format##_to_RGBA8888(benchmark::State& state) { \
		VTF vtf{fs::readFileBuffer(ASSET_ROOT "vtfpp/fmt/" #Format ".vtf")}; \
		for ([[maybe_unused]] auto _: state) { \
			benchmark::DoNotOptimize(vtf.getImageDataAsRGBA8888()); \
		} \
	} \
	BENCHMARK(BM_vtfpp_convert_256x256_##Format##_to_RGBA8888); \
	void BM_vtflib_convert_256x256_##Format##_to_RGBA8888(benchmark::State& state) { \
		VTFLib::CVTFFile vtf; \
		vtf.Load(ASSET_ROOT "vtfpp/fmt/" #Format ".vtf"); \
		for ([[maybe_unused]] auto _: state) { \
			std::vector<std::byte> data(vtf.GetWidth() * vtf.GetHeight() * sizeof(ImagePixel::RGBA8888)); \
			benchmark::DoNotOptimize(VTFLib::CVTFFile::ConvertToRGBA8888(vtf.GetData(0, 0, 0, 0), reinterpret_cast<vlByte*>(data.data()), vtf.GetWidth(), vtf.GetHeight(), IMAGE_FORMAT_##VTFLibFormat)); \
		} \
	} \
	BENCHMARK(BM_vtflib_convert_256x256_##Format##_to_RGBA8888)

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
BM_FORMAT(RGBA16161616F,      RGBA16161616F);
BM_FORMAT(RGBA16161616,       RGBA16161616);
BM_FORMAT(R32F,               R32F);
BM_FORMAT(RGB323232F,         RGB323232F);
BM_FORMAT(RGBA32323232F,      RGBA32323232F);

void BM_vtfpp_create_2048x2048_basic(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(VTF::create(imageData, ImageFormat::RGBA8888, 2048, 2048, {
			.outputFormat = ImageFormat::RGBA8888,
		}));
	}
}
BENCHMARK(BM_vtfpp_create_2048x2048_basic);

void BM_vtflib_create_2048x2048_basic(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		VTFLib::CVTFFile vtf;
		vtf.Create(2048, 2048, reinterpret_cast<vlByte*>(imageData.data()), {
			.uiVersion = {7, 4},
			.ImageFormat = IMAGE_FORMAT_RGBA8888,
			.bMipmaps = true,
			.bThumbnail = true,
			.bReflectivity = true,
		});
		std::vector<std::byte> data(vtf.GetSize());
		benchmark::DoNotOptimize(vtf.Save(data.data()));
		benchmark::DoNotOptimize(data);
	}
}
BENCHMARK(BM_vtflib_create_2048x2048_basic);

void BM_vtfpp_create_2048x2048_compressed(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		benchmark::DoNotOptimize(VTF::create(imageData, ImageFormat::RGBA8888, 2048, 2048, {
			.minorVersion = 6,
			.outputFormat = ImageFormat::RGBA8888,
			.compressionLevel = 9,
		}));
	}
}
BENCHMARK(BM_vtfpp_create_2048x2048_compressed);

void BM_vtflib_create_2048x2048_compressed(benchmark::State& state) {
	VTF image{ASSET_ROOT "vtfpp/fmt/RGBA8888.vtf"};
	auto imageData = image.getImageDataAsRGBA8888();

	for ([[maybe_unused]] auto _: state) {
		VTFLib::CVTFFile vtf;
		vtf.Create(2048, 2048, reinterpret_cast<vlByte*>(imageData.data()), {
			.uiVersion = {7, 6},
			.ImageFormat = IMAGE_FORMAT_RGBA8888,
			.bMipmaps = true,
			.bThumbnail = true,
			.bReflectivity = true,
		});
		vtf.SetAuxCompressionLevel(9);
		// This is the incorrect size, but I literally do not know how to use this library, and it shouldn't go over
		std::vector<std::byte> data(vtf.GetSize());
		benchmark::DoNotOptimize(vtf.Save(data.data()));
		benchmark::DoNotOptimize(data);
	}
}
BENCHMARK(BM_vtflib_create_2048x2048_compressed);

} // namespace
