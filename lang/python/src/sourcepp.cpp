#include "sourcepp.h"

#ifdef BSPPP
#include "bsppp.h"
#endif

#ifdef GAMEPP
#include "gamepp.h"
#endif

#ifdef STEAMPP
#include "steampp.h"
#endif

#ifdef TOOLPP
#include "toolpp.h"
#endif

#ifdef VCRYPTPP
#include "vcryptpp.h"
#endif

#ifdef VPKPP
#include "vpkpp.h"
#endif

#ifdef VTFPP
#include "vtfpp.h"
#endif

NB_MODULE(_sourcepp_impl, m) {
	m.doc() = "SourcePP: A Python wrapper around several modern C++20 libraries for sanely parsing Valve's formats.";

	sourcepp::register_python(m);

#ifdef GAMEPP
	gamepp::register_python(m);
#else
	m.def_submodule("gamepp");
#endif

#ifdef STEAMPP
	steampp::register_python(m);
#else
	m.def_submodule("steampp");
#endif

#ifdef TOOLPP
	toolpp::register_python(m);
#else
	m.def_submodule("toolpp");
#endif

#ifdef VCRYPTPP
	vcryptpp::register_python(m);
#else
	m.def_submodule("vcryptpp");
#endif

#ifdef VPKPP
	vpkpp::register_python(m);
#else
	m.def_submodule("vpkpp");
#endif

#ifdef BSPPP
	bsppp::register_python(m);
#else
	m.def_submodule("bsppp");
#endif

#ifdef VTFPP
	vtfpp::register_python(m);
#else
	m.def_submodule("vtfpp");
#endif
}
