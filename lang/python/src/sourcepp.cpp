#include "sourcepp.h"

#ifdef SOURCEPP_BSPPP
#include "bsppp.h"
#endif

#ifdef SOURCEPP_GAMEPP
#include "gamepp.h"
#endif

#ifdef SOURCEPP_KVPP
#include "kvpp.h"
#endif

#ifdef SOURCEPP_STEAMPP
#include "steampp.h"
#endif

#ifdef SOURCEPP_TOOLPP
#include "toolpp.h"
#endif

#ifdef SOURCEPP_VCRYPTPP
#include "vcryptpp.h"
#endif

#ifdef SOURCEPP_VPKPP
#include "vpkpp.h"
#endif

#ifdef SOURCEPP_VTFPP
#include "vtfpp.h"
#endif

NB_MODULE(_sourcepp_impl, m) {
	m.doc() = "SourcePP: A Python wrapper around several modern C++20 libraries for sanely parsing Valve's formats.";

	sourcepp::register_python(m);

#ifdef SOURCEPP_GAMEPP
	gamepp::register_python(m);
#endif

#ifdef SOURCEPP_KVPP
	kvpp::register_python(m);
#endif

#ifdef SOURCEPP_STEAMPP
	steampp::register_python(m);
#endif

#ifdef SOURCEPP_TOOLPP
	toolpp::register_python(m);
#endif

#ifdef SOURCEPP_VCRYPTPP
	vcryptpp::register_python(m);
#endif

#ifdef SOURCEPP_VPKPP
	vpkpp::register_python(m);
#endif

#ifdef SOURCEPP_BSPPP
	bsppp::register_python(m);
#endif

#ifdef SOURCEPP_VTFPP
	vtfpp::register_python(m);
#endif
}
