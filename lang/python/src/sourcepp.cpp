#include "sourcepp.h"

#ifdef BSPPP
#include "bsppp.h"
#endif

#ifdef GAMEPP
#include "gamepp.h"
#endif

#ifdef KVPP
#include "kvpp.h"
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
#endif

#ifdef KVPP
	kvpp::register_python(m);
#endif

#ifdef STEAMPP
	steampp::register_python(m);
#endif

#ifdef TOOLPP
	toolpp::register_python(m);
#endif

#ifdef VCRYPTPP
	vcryptpp::register_python(m);
#endif

#ifdef VPKPP
	vpkpp::register_python(m);
#endif

#ifdef BSPPP
	bsppp::register_python(m);
#endif

#ifdef VTFPP
	vtfpp::register_python(m);
#endif
}
