#pragma once

#include <squirrel.h>
#include <../sqrat/include/sqrat.h>

namespace sq = Sqrat;

#include <vcryptpp/vcryptpp.h>

namespace vcryptpp {

inline void register_quirrel(HSQUIRRELVM vm, sq::Table& p) {
	sq::Table vcryptpp{vm};

	{
		using namespace VFONT;
		sq::Table vfont{vm};

		vfont.SetValue("SIGNATURE", SIGNATURE);
		vfont.SetValue("MAGIC", MAGIC);

		vfont.Func("encrypt_bytes", [](const SQChar* data, SQInteger saltSize) -> Sqrat::string {
			std::string_view dv{data};
			const auto d = encrypt({reinterpret_cast<const std::byte*>(dv.data()), dv.size()}, static_cast<uint8_t>(saltSize));
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		});

		vfont.Func("decrypt_bytes", [](const SQChar* data) -> Sqrat::string {
			std::string_view dv{data};
			const auto d = decrypt({reinterpret_cast<const std::byte*>(dv.data()), dv.size()});
			return {reinterpret_cast<const char*>(d.data()), d.size()};
		});

		vcryptpp.Bind("VFONT", vfont);
	}

	p.Bind("vcryptpp", vcryptpp);
}

} // namespace vcryptpp
