#include "sourcepp.h"

#ifdef GAMEPP
#include "gamepp.h"
#endif

#ifdef STEAMPP
#include "steampp.h"
#endif

PYBIND11_MODULE(sourcepp_python, m) {
	m.doc() = "SourcePP: A Python wrapper around several modern C++20 libraries for sanely parsing Valve's formats.";

	m.attr("__version__") = "dev";

	sourcepp::register_python(m);

#ifdef GAMEPP
	gamepp::register_python(m);
#endif

#ifdef STEAMPP
	steampp::register_python(m);
#endif
}
