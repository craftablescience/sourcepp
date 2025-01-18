#pragma once

/*
 * This is a header designed to be included in C++ source code.
 * It should not be included in applications using any C wrapper libraries!
 */
#ifndef __cplusplus
#error "This header can only be used in C++!"
#endif

namespace vtfpp {

class VTF;

} // namespace vpkpp

namespace Convert {

vtfpp::VTF* vtf(vtfpp_vtf_handle_t handle);

} // namespace Convert
