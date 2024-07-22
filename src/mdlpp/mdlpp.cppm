module;

#include <mdlpp/mdlpp.h>

export module mdlpp;

export {

	// structs/Generic.h
	namespace mdlpp {
		constexpr auto MDLPP_ROOT_LOD = ROOT_LOD;
		constexpr auto MDLPP_MAX_LOD_COUNT = MAX_LOD_COUNT;
		constexpr auto MDLPP_MAX_BONES_PER_VERTEX = MAX_BONES_PER_VERTEX;
		using mdlpp::BBox;
		using mdlpp::Movement;
	} // namespace mdlpp

	// structs/MDL.h
	namespace mdlpp::MDL {
		using mdlpp::MDL::Bone;
		using mdlpp::MDL::BoneController;
		using mdlpp::MDL::HitboxSet;
		using mdlpp::MDL::Material;
		using mdlpp::MDL::Mesh;
		using mdlpp::MDL::Model;
		using mdlpp::MDL::BodyPart;
		using mdlpp::MDL::MDL;
	} // namespace mdlpp::MDL

	// structs/VTX.h
	namespace mdlpp::VTX {
		using mdlpp::VTX::Vertex;
		using mdlpp::VTX::Strip;
		using mdlpp::VTX::StripGroup;
		using mdlpp::VTX::Mesh;
		using mdlpp::VTX::ModelLOD;
		using mdlpp::VTX::Model;
		using mdlpp::VTX::BodyPart;
		using mdlpp::VTX::VTX;
	} // namespace mdlpp::VTX

	// structs/VVD.h
	namespace mdlpp::VVD {
		using mdlpp::VVD::Fixup;
		using mdlpp::VVD::BoneWeight;
		using mdlpp::VVD::Vertex;
		using mdlpp::VVD::VVD;
	} // namespace mdlpp::VVD

	// mdlpp.h
	namespace mdlpp {
		using mdlpp::StudioModel;
	} // namespace mdlpp

} // export
