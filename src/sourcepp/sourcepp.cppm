module;

#include <sourcepp/crypto/Adler32.h>
#include <sourcepp/crypto/CRC32.h>
#include <sourcepp/crypto/MD5.h>
#include <sourcepp/crypto/RSA.h>
#include <sourcepp/crypto/String.h>
#include <sourcepp/fs/FS.h>
#include <sourcepp/math/Angles.h>
#include <sourcepp/math/Float.h>
#include <sourcepp/math/Integer.h>
#include <sourcepp/math/Matrix.h>
#include <sourcepp/math/Vector.h>
#include <sourcepp/parser/Binary.h>
#include <sourcepp/parser/Text.h>
#include <sourcepp/string/String.h>

export module sourcepp;

export {

	// crypto/Adler32.h
	namespace sourcepp::crypto {
		using sourcepp::crypto::computeAdler32;
	} // namespace sourcepp::crypto

	// crypto/CRC32.h
	namespace sourcepp::crypto {
		using sourcepp::crypto::computeCRC32;
	} // namespace sourcepp::crypto

	// crypto/MD5.h
	namespace sourcepp::crypto {
		using sourcepp::crypto::computeMD5;
	} // namespace sourcepp::crypto

	// crypto/RSA.h
	namespace sourcepp::crypto {
		using sourcepp::crypto::computeSHA256KeyPair;
		using sourcepp::crypto::verifySHA256PublicKey;
		using sourcepp::crypto::signDataWithSHA256PrivateKey;
	} // namespace sourcepp::crypto

	// crypto/String.h
	namespace sourcepp::crypto {
		using sourcepp::crypto::decodeHexString;
	} // namespace sourcepp::crypto

	// fs/FS.h
	namespace sourcepp::fs {
		using sourcepp::fs::readFileBuffer;
		using sourcepp::fs::readFileText;
	} // namespace sourcepp::fs

	// math/Angles.h
	namespace sourcepp::math {
		using sourcepp::math::EulerAngles;
		using sourcepp::math::Quat;
		using sourcepp::math::QuatCompressed48;
		using sourcepp::math::QuatCompressed64;
	} // namespace sourcepp::math

	// math/Float.h
	namespace sourcepp::math {
		using sourcepp::math::FloatCompressed16;
	} // namespace sourcepp::math

	// math/Integer.h
	using std::int8_t;
	using std::int16_t;
	using std::int32_t;
	using std::int64_t;
	using std::uint8_t;
	using std::uint16_t;
	using std::uint32_t;
	using std::uint64_t;
	namespace sourcepp::math {
		using sourcepp::math::Arithmetic;
	} // namespace sourcepp::math

	// math/Matrix.h
	namespace sourcepp::math {
		using sourcepp::math::Matrix;
	} // namespace sourcepp::math

	// math/Vector.h
	namespace sourcepp::math {
		using sourcepp::math::Vec2;
		using sourcepp::math::Vec2i8;
		using sourcepp::math::Vec2i16;
		using sourcepp::math::Vec2i32;
		using sourcepp::math::Vec2i64;
		using sourcepp::math::Vec2i;
		using sourcepp::math::Vec2ui8;
		using sourcepp::math::Vec2ui16;
		using sourcepp::math::Vec2ui32;
		using sourcepp::math::Vec2ui64;
		using sourcepp::math::Vec2ui;
		using sourcepp::math::Vec2f32;
		using sourcepp::math::Vec2f64;
		using sourcepp::math::Vec2f;
		using sourcepp::math::Vec3;
		using sourcepp::math::Vec3i8;
		using sourcepp::math::Vec3i16;
		using sourcepp::math::Vec3i32;
		using sourcepp::math::Vec3i64;
		using sourcepp::math::Vec3i;
		using sourcepp::math::Vec3ui8;
		using sourcepp::math::Vec3ui16;
		using sourcepp::math::Vec3ui32;
		using sourcepp::math::Vec3ui64;
		using sourcepp::math::Vec3ui;
		using sourcepp::math::Vec3f32;
		using sourcepp::math::Vec3f64;
		using sourcepp::math::Vec3f;
		using sourcepp::math::Vec4;
		using sourcepp::math::Vec4i8;
		using sourcepp::math::Vec4i16;
		using sourcepp::math::Vec4i32;
		using sourcepp::math::Vec4i64;
		using sourcepp::math::Vec4i;
		using sourcepp::math::Vec4ui8;
		using sourcepp::math::Vec4ui16;
		using sourcepp::math::Vec4ui32;
		using sourcepp::math::Vec4ui64;
		using sourcepp::math::Vec4ui;
		using sourcepp::math::Vec4f32;
		using sourcepp::math::Vec4f64;
		using sourcepp::math::Vec4f;
	} // namespace sourcepp::math

	// parser/Binary.h
	namespace sourcepp::parser::binary {
		using sourcepp::parser::binary::makeFourCC;
		using sourcepp::parser::binary::readStringAtOffset;
	} // namespace sourcepp::parser::binary

	// parser/Text.h
	namespace sourcepp::parser::text {
		constexpr auto SOURCEPP_DEFAULT_SINGLE_LINE_COMMENT_START = DEFAULT_SINGLE_LINE_COMMENT_START;
		constexpr auto SOURCEPP_DEFAULT_MULTI_LINE_COMMENT_START = DEFAULT_MULTI_LINE_COMMENT_START;
		constexpr auto SOURCEPP_DEFAULT_MULTI_LINE_COMMENT_END = DEFAULT_MULTI_LINE_COMMENT_END;
		constexpr auto SOURCEPP_DEFAULT_STRING_START = DEFAULT_STRING_START;
		constexpr auto SOURCEPP_DEFAULT_STRING_END = DEFAULT_STRING_END;
		const auto& SOURCEPP_DEFAULT_ESCAPE_SEQUENCES = sourcepp::parser::text::DEFAULT_ESCAPE_SEQUENCES;
		const auto& SOURCEPP_NO_ESCAPE_SEQUENCES =  sourcepp::parser::text::NO_ESCAPE_SEQUENCES;
		using sourcepp::parser::text::isNewLine;
		using sourcepp::parser::text::isWhitespace;
		using sourcepp::parser::text::isNumber;
		using sourcepp::parser::text::eatWhitespace;
		using sourcepp::parser::text::eatSingleLineComment;
		using sourcepp::parser::text::eatMultiLineComment;
		using sourcepp::parser::text::eatWhitespaceAndSingleLineComments;
		using sourcepp::parser::text::eatWhitespaceAndMultiLineComments;
		using sourcepp::parser::text::eatWhitespaceAndComments;
		using sourcepp::parser::text::tryToEatChar;
		using sourcepp::parser::text::readStringToBuffer;
		using sourcepp::parser::text::readUnquotedStringToBuffer;
		using sourcepp::parser::text::syntax_error;
	} // namespace sourcepp::parser::text

	// string/String.h
	namespace sourcepp::string {
		using sourcepp::string::contains;
		using sourcepp::string::iequals;
		using sourcepp::string::ltrim;
		using sourcepp::string::rtrim;
		using sourcepp::string::trim;
		using sourcepp::string::split;
		using sourcepp::string::toLower;
		using sourcepp::string::toUpper;
		using sourcepp::string::createRandom;
		using sourcepp::string::generateUUIDv4;
		using sourcepp::string::padNumber;
		using sourcepp::string::normalizeSlashes;
		using sourcepp::string::denormalizeSlashes;
	} // namespace sourcepp::string

} // export
