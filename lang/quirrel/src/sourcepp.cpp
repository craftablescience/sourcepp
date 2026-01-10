#include <sourcepp_quirrel.h>

#include <../sqrat/include/sqrat.h>

#ifdef SOURCEPP_VCRYPTPP
#include "vcryptpp.h"
#endif

namespace sourcepp {

void quirrel::bind(HSQUIRRELVM vm) {
	sq::Table p{vm};

#ifdef SOURCEPP_VCRYPTPP
	vcryptpp::register_quirrel(vm, p);
#endif

	sq::RootTable(vm).Bind("sourcepp", p);
}

} // namespace sourcepp
