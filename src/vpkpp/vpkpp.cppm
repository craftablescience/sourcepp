module;

#include <vpkpp/format/BSP.h>
#include <vpkpp/format/FPX.h>
#include <vpkpp/format/GCF.h>
#include <vpkpp/format/GMA.h>
#include <vpkpp/format/GRP.h>
#include <vpkpp/format/PAK.h>
#include <vpkpp/format/PCK.h>
#include <vpkpp/format/VPK.h>
#include <vpkpp/format/ZIP.h>
#include <vpkpp/Attribute.h>
#include <vpkpp/Entry.h>
#include <vpkpp/Options.h>
#include <vpkpp/PackFile.h>
#include <vpkpp/PackFileType.h>

export module vpkpp;

export {

	// format/BSP.h
	namespace vpkpp {
		using vpkpp::BSP;
	} // namespace vpkpp

	// format/FPX.h
	namespace vpkpp {
		using vpkpp::FPX;
	} // namespace vpkpp

	// format/GCF.h
	namespace vpkpp {
		using vpkpp::GCF;
	} // namespace vpkpp

	// format/GMA.h
	namespace vpkpp {
		using vpkpp::GMA;
	} // namespace vpkpp

	// format/GRP.h
	namespace vpkpp {
		using vpkpp::GRP;
	} // namespace vpkpp

	// format/PAK.h
	namespace vpkpp {
		using vpkpp::PAK;
	} // namespace vpkpp

	// format/PCK.h
	namespace vpkpp {
		using vpkpp::PCK;
	} // namespace vpkpp

	// format/VPK.h
	namespace vpkpp {
		using vpkpp::VPK;
	} // namespace vpkpp

	// format/ZIP.h
	namespace vpkpp {
		using vpkpp::ZIP;
	} // namespace vpkpp

	// Attribute.h
	namespace vpkpp {
		using vpkpp::Attribute;
	} // namespace vpkpp

	// Entry.h
	namespace vpkpp {
		using vpkpp::Entry;
	} // namespace vpkpp

	// Options.h
	namespace vpkpp {
		using vpkpp::PackFileOptions;
		using vpkpp::EntryOptions;
	} // namespace vpkpp

	// PackFile.h
	namespace vpkpp {
		using vpkpp::PackFile;
		using vpkpp::PackFileReadOnly;
	} // namespace vpkpp

	// PackFileType.h
	namespace vpkpp {
		using vpkpp::PackFileType;
	} // namespace vpkpp

} // export
