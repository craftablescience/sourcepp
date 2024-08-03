#pragma once

#include <string>
#include <vector>

#include <sourcepp/math/Vector.h>

namespace sourcepp::mesh {

struct VertexStatic {
	math::Vec3f position;
	math::Vec3f normal;
	math::Vec2f uv1;
	math::Vec2f uv2;
};

class MeshBuilderStatic {
public:
	MeshBuilderStatic() = default;

	MeshBuilderStatic& addVertex(VertexStatic v);

	MeshBuilderStatic& finishFace();

	[[nodiscard]] const std::vector<VertexStatic>& getVertices() const;

	[[nodiscard]] std::string toOBJ() const;

private:
	// Stored as a triangle list
	std::vector<VertexStatic> vertices;
	std::vector<VertexStatic> incompleteFace;
};

} // namespace sourcepp::mesh
