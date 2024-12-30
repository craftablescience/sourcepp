/**
uc::apng::loader
Copyright (c) 2017, Kentaro Ushiyama
This software is released under the MIT License.
http://opensource.org/licenses/mit-license.php
*/
#ifndef UC_APNG_LOADER_H
#define UC_APNG_LOADER_H
#define UC_APNG_LOADER_VERSION "1.0.4"
#define UC_APNG_LOADER_VERSION_NUM 0x000103
#ifdef _MSC_VER
# define _SCL_SECURE_NO_WARNINGS
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#ifndef STBI_INCLUDE_STB_IMAGE_H
# include "stb_image.h"
#endif

#ifndef UC_APNG_LOADER_NO_EXCEPTION
# define UC_APNG_ASSERT(pred) if (!(pred)) throw uc::apng::exception(std::string(__func__).append(" : ").append(#pred).append(" failed."))
#else
# define UC_APNG_ASSERT(pred)
#endif

namespace uc {
    namespace apng {

        template <typename T> constexpr T swap_byteorder(T v) noexcept;
        template <> constexpr uint8_t swap_byteorder(uint8_t v) noexcept
        {
            return v;
        }
        template <> constexpr uint16_t swap_byteorder(uint16_t v) noexcept
        {
            return ((v & 0xFF) << 8) | ((v & 0xFF00) >> 8);
        }
        template <> constexpr uint32_t swap_byteorder(uint32_t v) noexcept
        {
            return ((v & 0xFF) << 24) | ((v & 0xFF00) << 8) | ((v & 0xFF0000) >> 8) | ((v & 0xFF000000) >> 24);
        }
#ifdef IS_BIG_ENDIAN
        template <typename T> constexpr T net_to_host(T v) noexcept {return v;}
#else
        template <typename T> constexpr T net_to_host(T v) noexcept {return swap_byteorder(v);}
#endif
        template <typename T> constexpr T host_to_net(T v) noexcept {return net_to_host(v);}
        template <typename T> void set_to_binary(char* data, T val) noexcept {*reinterpret_cast<T*>(data) = host_to_net(val);}
        template <typename T> void set_to_binary(uint8_t* data, T val) noexcept {*reinterpret_cast<T*>(data) = host_to_net(val);}
        template <typename T> T get_from_binary(const char* data) noexcept {return net_to_host(*reinterpret_cast<const T*>(data));}
        template <typename T> T get_from_binary(const uint8_t* data) noexcept {return net_to_host(*reinterpret_cast<const T*>(data));}
        inline bool load_from_binary(const uint8_t* head, const uint8_t* tail) noexcept
        {
            return head <= tail;
        }
        template <typename T, typename... Ts> bool load_from_binary(const uint8_t* head, const uint8_t* tail, T& param, Ts&&... params) noexcept
        {
            if (head + sizeof(T) > tail) return false;
            param = get_from_binary<T>(head);
            return load_from_binary(head + sizeof(T), tail, std::forward<Ts>(params)...);
        }


        class exception : public std::runtime_error
        {
        public:
            explicit exception(const char* what) : std::runtime_error(what) {}
            explicit exception(const std::string& what) : std::runtime_error(what) {}
        };


        using signature_t = std::array<uint8_t, 8>;
        constexpr const signature_t SIGNATURE {{137, 80, 78, 71, 13, 10, 26, 10}};
        constexpr const std::array<uint8_t, 12> IEND_CHUNK {{0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130}};


        constexpr const size_t CHUNK_HEADER_SIZE = 8; // = sizeof(payload_size) + sizeof(type)
        constexpr const size_t CHUNK_FOOTER_SIZE = 4; // = sizeof(CRC)
        constexpr const size_t CHUNK_HEADER_FOOTER_SIZE = CHUNK_HEADER_SIZE + CHUNK_FOOTER_SIZE;

        inline uint32_t get_chunk_payload_size(const std::vector<uint8_t>& chunk) noexcept
        {
            return get_from_binary<uint32_t>(chunk.data());
        }
        inline uint32_t get_chunk_type(const std::vector<uint8_t>& chunk) noexcept
        {
            return get_from_binary<uint32_t>(chunk.data() + 4);
        }
        inline std::string get_chunk_name(const std::vector<uint8_t>& chunk)
        {
            return std::string(reinterpret_cast<const char*>(chunk.data() + 4), 4);
        }
        template <typename... Ts> bool parse_chunk(const std::vector<uint8_t>& chunk, uint32_t type, Ts&&... params) noexcept
        {
            return (get_chunk_type(chunk) == type) && load_from_binary(chunk.data() + CHUNK_HEADER_SIZE, chunk.data() + chunk.size(), std::forward<Ts>(params)...);
        }
        inline std::vector<uint8_t> read_chunk(std::istream& is)
        {
            std::vector<uint8_t> chunk;
            chunk.resize(4);
            is.read(reinterpret_cast<char*>(chunk.data()), 4);
            UC_APNG_ASSERT(!is.fail());
            chunk.resize(get_chunk_payload_size(chunk) + CHUNK_HEADER_FOOTER_SIZE);
            is.read(reinterpret_cast<char*>(chunk.data() + 4), chunk.size() - 4);
            UC_APNG_ASSERT(!is.fail());
            return chunk;
        }


        namespace type
        {
            constexpr uint32_t IHDR = 0x49484452;
            constexpr uint32_t IDAT = 0x49444154;
            constexpr uint32_t IEND = 0x49454E44;
            constexpr uint32_t acTL = 0x6163544C;
            constexpr uint32_t fcTL = 0x6663544C;
            constexpr uint32_t fdAT = 0x66644154;
        }


        struct IHDR_payload_t
        {
            uint32_t width;
            uint32_t height;
            uint8_t bit_depth;
            uint8_t color_type;
            uint8_t complession_method;
            uint8_t filter_method;
            uint8_t interlace_method;
        };
        inline IHDR_payload_t parse_as_IHDR(const std::vector<uint8_t>& chunk)
        {
            IHDR_payload_t data = {};
            UC_APNG_ASSERT(chunk.size() == 25);
            UC_APNG_ASSERT(parse_chunk(chunk, type::IHDR, data.width, data.height, data.bit_depth, data.color_type, data.complession_method, data.filter_method, data.interlace_method));
            return data;
        }


        struct acTL_payload_t
        {
            uint32_t num_frames = 0;	//!< Number of frames
            uint32_t num_plays = 0;		//!< Number of times to loop this APNG.  0 indicates infinite looping.
        };
        inline acTL_payload_t parse_as_acTL(const std::vector<uint8_t>& chunk)
        {
            acTL_payload_t data {};
            UC_APNG_ASSERT(chunk.size() == 20);
            UC_APNG_ASSERT(parse_chunk(chunk, type::acTL, data.num_frames, data.num_plays));
            UC_APNG_ASSERT(data.num_frames > 0);
            return data;
        }


        enum dispose_op_t : uint8_t
        {
            NONE,
            BACKGROUND,
            PREVIOUS
        };
        enum blend_op_t : uint8_t
        {
            SOURCE,
            OVER
        };
        struct fcTL_payload_t
        {
            uint32_t sequence_number;	//!< Sequence number of the animation chunk, starting from 0
            uint32_t width;		 		//!< Width of the following frame
            uint32_t height;		 	//!< Height of the following frame
            uint32_t x_offset;		 	//!< X position at which to render the following frame
            uint32_t y_offset;		 	//!< Y position at which to render the following frame
            uint16_t delay_num;		 	//!< Frame delay fraction numerator
            uint16_t delay_den;		 	//!< Frame delay fraction denominator
            dispose_op_t dispose_op;	//!< Type of frame area disposal to be done after rendering this frame
            blend_op_t blend_op;		//!< Type of frame area rendering for this frame
        };
        inline fcTL_payload_t parse_as_fcTL(const std::vector<uint8_t>& chunk)
        {
            fcTL_payload_t data {};
            uint8_t dispose, blend;
            UC_APNG_ASSERT(chunk.size() == 38);
            UC_APNG_ASSERT(parse_chunk(chunk, type::fcTL, data.sequence_number, data.width, data.height, data.x_offset, data.y_offset, data.delay_num, data.delay_den, dispose, blend));
            UC_APNG_ASSERT(dispose <= 2);
            UC_APNG_ASSERT(blend <= 1);
            data.dispose_op = static_cast<dispose_op_t>(dispose);
            data.blend_op =  static_cast<blend_op_t>(blend);
            // If the denominator is 0, it is to be treated as if it were 100.
            if (data.delay_den == 0) {
                data.delay_den = 100;
            }
            return data;
        }


        struct stbi_deleter
        {
            void operator()(stbi_uc* v) const noexcept
            {
                stbi_image_free(v);
            }
        };
        using stbi_ptr = std::unique_ptr<stbi_uc, stbi_deleter>;


        class image_t
        {
        public:
            constexpr static size_t BPP = 4;

            image_t() = default;
            image_t(image_t&&) = default;
            image_t& operator=(image_t&&) = default;
            ~image_t() = default;

            image_t(const image_t& obj)
                    : width_(obj.width_), height_(obj.height_), bin(static_cast<stbi_uc*>(malloc(size())))
            {
                std::copy(obj.begin(), obj.end(), begin());
            }
            image_t& operator=(const image_t& obj)
            {
                if (this != &obj) {
                    *this = image_t(obj);
                }
                return *this;
            }
            image_t(uint32_t w, uint32_t h)
                    : width_(w), height_(h), bin(static_cast<stbi_uc*>(malloc(size())))
            {
                std::fill(begin(), end(), 0);
            }
            image_t(std::vector<uint8_t>& pngBinData)
            {
                int w, h, d;
                bin = stbi_ptr(stbi_load_from_memory(pngBinData.data(), static_cast<int>(pngBinData.size()), &w, &h, &d, STBI_rgb_alpha));
                width_ = w;
                height_ = h;
                UC_APNG_ASSERT(d == BPP);
            }

            explicit operator bool() const noexcept
            {
                return static_cast<bool>(bin);
            }
            uint32_t size() const noexcept
            {
                return width_ * height_ * BPP;
            }
            uint32_t width() const noexcept
            {
                return width_;
            }
            uint32_t height() const noexcept
            {
                return height_;
            }
            uint32_t offset(uint32_t x, uint32_t y) const noexcept
            {
                return (y * width_ + x) * BPP;
            }

            uint8_t* begin() noexcept
            {
                return data();
            }
            const uint8_t* begin() const noexcept
            {
                return data();
            }
            uint8_t* end() noexcept
            {
                return data() + size();
            }
            const uint8_t* end() const noexcept
            {
                return data() + size();
            }

            uint8_t* data() noexcept
            {
                return bin.get();
            }
            const uint8_t* data() const noexcept
            {
                return bin.get();
            }
        private:
            uint32_t width_ = 0;
            uint32_t height_ = 0;
            stbi_ptr bin {};
        };
        inline void copy_frame(const image_t& src, image_t& dst, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        {
            for (uint32_t j = 0; j < h; j++) {
                std::copy(src.data() + src.offset(0, j), src.data() + src.offset(w, j), dst.data() + dst.offset(x, j + y));
            }
        }
        inline void over_frame(const image_t& src, image_t& dst, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        {
            for (uint32_t j = 0; j < h; j++) {
                auto sp = src.data() + src.offset(0, j);
                auto dp = dst.data() + dst.offset(x, j + y);
                for (uint32_t i = 0; i < w; ++i, sp += 4, dp += 4) {
                    if (sp[3] == 0) {
                        continue;
                    } else if ((sp[3] == 255) || (dp[3] == 0)) {
                        std::copy(sp, sp + 4, dp);
                    } else {
                        int u = sp[3]*255;
                        int v = (255-sp[3])*dp[3];
                        int al = u + v;
                        dp[0] = (sp[0]*u + dp[0]*v)/al;
                        dp[1] = (sp[1]*u + dp[1]*v)/al;
                        dp[2] = (sp[2]*u + dp[2]*v)/al;
                        dp[3] = al/255;
                    }
                }
            }
        }
        inline void blend_frame(const image_t& src, image_t& dst, const fcTL_payload_t& fcTL)
        {
            if (fcTL.blend_op == blend_op_t::SOURCE) {
                copy_frame(src, dst, fcTL.x_offset, fcTL.y_offset, fcTL.width, fcTL.height);
            } else {
                over_frame(src, dst, fcTL.x_offset, fcTL.y_offset, fcTL.width, fcTL.height);
            }
        }


        struct frame
        {
            size_t index;		//!< frame index
            image_t image;		//!< raw 32bit RGBA image
            uint16_t delay_num;	//!< Frame delay fraction numerator
            uint16_t delay_den;	//!< Frame delay fraction denominator
            bool is_default;	//!< true if this image is IDAT frame
        };

        template <typename InputStream> class loader
        {
        public:
            loader() = default;
            loader(loader&&) = default;
            loader(const loader&) = delete;
            loader& operator=(loader&&) = default;
            loader& operator=(const loader&) = delete;

            loader(std::unique_ptr<InputStream>&& stream) : stream_(std::move(stream))
            {
                signature_t sig {};
                stream_->read(reinterpret_cast<char*>(sig.data()), sig.size());
                UC_APNG_ASSERT(!stream_->fail());
                UC_APNG_ASSERT(sig == SIGNATURE);

                IHDRchunk = read_chunk(*stream_);
                IHDRpayload = parse_as_IHDR(IHDRchunk);
                while ((nowState != state::acTL_LOADED) && !eof()) {
                    load_one_chunk();
                }
                if (eof() && !IDATchunk.empty()) {
                    acTLpayload.num_frames = 1;	// Normal PNG file loaded.
                }
            }
            ~loader() = default;
            //! APNG image width
            uint32_t width() const noexcept
            {
                return IHDRpayload.width;
            }
            //! APNG image height
            uint32_t height() const noexcept
            {
                return IHDRpayload.height;
            }

            //! Number of frames
            uint32_t num_frames() const noexcept
            {
                return acTLpayload.num_frames;
            }
            //! Number of times to loop this APNG.  0 indicates infinite looping.
            uint32_t num_plays() const noexcept
            {
                return acTLpayload.num_plays;
            }
            bool has_frame() const noexcept
            {
                return frameIndex < num_frames();
            }
            frame next_frame()
            {
                UC_APNG_ASSERT(has_frame());
                while ((nowState != state::fdAT_LOADED) && !eof()) {
                    load_one_chunk();
                }
                frame ret {frameIndex++, construct_image(), fcTLpayload.delay_num, fcTLpayload.delay_den, IDATLoaded};

                if (nowState == state::fdAT_LOADED) {
                    nowState = state::acTL_LOADED;
                    image_t newFrame = std::move(frameBuffer);
                    if (!newFrame) {
                        newFrame = image_t(width(), height());
                    }
                    switch (fcTLpayload.dispose_op) {
                        // no disposal is done on this frame before rendering the next; the contents of the output buffer are left as is.
                        case dispose_op_t::NONE:
                            blend_frame(ret.image, newFrame, fcTLpayload);
                            frameBuffer = newFrame;
                            break;
                            // the frame's region of the output buffer is to be cleared to fully transparent black before rendering the next frame.
                        case dispose_op_t::BACKGROUND:
                            blend_frame(ret.image, newFrame, fcTLpayload);
                            frameBuffer = image_t(width(), height());
                            break;
                            // the frame's region of the output buffer is to be reverted to the previous contents before rendering the next frame.
                        case dispose_op_t::PREVIOUS:
                            frameBuffer = newFrame;
                            blend_frame(ret.image, newFrame, fcTLpayload);
                            break;
                        default:
                            throw exception("apng::next_frame : unknown apng::dispose_op_t");
                    }
                    ret.image = std::move(newFrame);
                }
                return ret;
            }

        private:
            enum class state
            {
                UNINITIALIZED,
                acTL_LOADED,
                fcTL_LOADED,
                fdAT_LOADED,
                IEND_LOADED
            };
            image_t construct_image() const
            {
                std::vector<uint8_t> data;
                data.reserve(SIGNATURE.size() + IHDRchunk.size() + IDATchunk.size() + otherChunks.size() + IEND_CHUNK.size());
                data.insert(data.end(), SIGNATURE.begin(), SIGNATURE.end());
                data.insert(data.end(), IHDRchunk.begin(), IHDRchunk.end());
                data.insert(data.end(), IDATchunk.begin(), IDATchunk.end());
                data.insert(data.end(), otherChunks.begin(), otherChunks.end());
                data.insert(data.end(), IEND_CHUNK.begin(), IEND_CHUNK.end());
                return image_t(data);
            }
            bool eof() const
            {
                return (nowState == state::IEND_LOADED) || stream_->eof();
            }
            void load_one_chunk()
            {
                auto chunk = read_chunk(*stream_);
                switch (get_chunk_type(chunk)) {
                    case type::IEND:
                        nowState = state::IEND_LOADED;
                        break;
                    case type::acTL:
                        if (nowState == state::UNINITIALIZED) {
                            nowState = state::acTL_LOADED;
                            acTLpayload = parse_as_acTL(chunk);
                        }
                        break;
                    case type::fcTL:
                        if (nowState != state::UNINITIALIZED) {
                            nowState = state::fcTL_LOADED;
                            fcTLpayload = parse_as_fcTL(chunk);
                            UC_APNG_ASSERT((fcTLpayload.x_offset + fcTLpayload.width) <= IHDRpayload.width);
                            UC_APNG_ASSERT((fcTLpayload.y_offset + fcTLpayload.height) <= IHDRpayload.height);
                            // modify IHDR chunk
                            set_to_binary<uint32_t>(IHDRchunk.data() + 8,  fcTLpayload.width);
                            set_to_binary<uint32_t>(IHDRchunk.data() + 12, fcTLpayload.height);
                            if (frameIndex == 0) {
                                fcTLpayload.blend_op = blend_op_t::SOURCE;
                                // If the first `fcTL` chunk uses a `dispose_op` of APNG_DISPOSE_OP_PREVIOUS it should be treated as APNG_DISPOSE_OP_BACKGROUND.
                                if (fcTLpayload.dispose_op == dispose_op_t::PREVIOUS) {
                                    fcTLpayload.dispose_op = dispose_op_t::BACKGROUND;
                                }
                            }
                        }
                        break;
                    case type::IDAT:
                        if (nowState == state::fcTL_LOADED) {
                            nowState = state::fdAT_LOADED;
                        }
                        IDATchunk = std::move(chunk);
                        IDATLoaded = true;
                        break;
                    case type::fdAT:
                        if (nowState == state::fcTL_LOADED) {
                            nowState = state::fdAT_LOADED;
                            // pretend to be IDAT chunk.
                            set_to_binary<uint32_t>(chunk.data() + 4, static_cast<uint32_t>(chunk.size() - 16));
                            set_to_binary<uint32_t>(chunk.data() + 8, type::IDAT);
                            IDATchunk.assign(chunk.begin() + 4, chunk.end());
                            IDATLoaded = false;
                        }
                        break;
                    default:
                        otherChunks.insert(otherChunks.end(), chunk.begin(), chunk.end());
                        break;
                }
            }

            std::unique_ptr<InputStream> stream_;
            state nowState = state::UNINITIALIZED;
            std::vector<uint8_t> IHDRchunk;
            std::vector<uint8_t> IDATchunk;
            std::vector<uint8_t> otherChunks;
            IHDR_payload_t IHDRpayload;
            acTL_payload_t acTLpayload {};
            fcTL_payload_t fcTLpayload {};
            image_t frameBuffer;
            size_t frameIndex = 0;
            bool IDATLoaded = false;
        };

        template <class T, typename... Args>
        std::unique_ptr<T> make_unique(Args&&... args)
        {
            return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        }
        inline loader<std::ifstream> create_file_loader(const std::string& filename)
        {
            return loader<std::ifstream>(uc::apng::make_unique<std::ifstream>(filename.c_str(), std::ios::in | std::ios::binary));
        }
#ifdef _MSC_VER
        inline loader<std::ifstream> create_file_loader(const std::wstring& filename)
{
	return loader<std::ifstream>(uc::apng::make_unique<std::ifstream>(filename.c_str(), std::ios::in | std::ios::binary));
}
#endif
        inline loader<std::istringstream> create_memory_loader(const std::string& data)
        {
            return loader<std::istringstream>(uc::apng::make_unique<std::istringstream>(data));
        }
        inline loader<std::istringstream> create_memory_loader(const char* data, size_t dataBytes)
        {
            return create_memory_loader(std::string(data, dataBytes));
        }

    }
}
#endif