#include <sourcepp/Mesh.h>

using namespace sourcepp::mesh;

MeshBuilderStatic& MeshBuilderStatic::addVertex(VertexStatic v) {
	this->incompleteFace.push_back(v);
	return *this;
}

MeshBuilderStatic& MeshBuilderStatic::finishFace() {
	for (int i = 2; i < this->incompleteFace.size(); i++) {
		this->vertices.push_back(this->incompleteFace[0]);
		this->vertices.push_back(this->incompleteFace[i - 1]);
		this->vertices.push_back(this->incompleteFace[i]);
	}
	this->incompleteFace.clear();
	return *this;
}

const std::vector<VertexStatic>& MeshBuilderStatic::getVertices() const {
	return this->vertices;
}

std::string MeshBuilderStatic::toOBJ() const {
	std::string out;
	for (const auto& vertex : this->vertices) {
		out += "v " + std::to_string(vertex.position[0]) + ' ' + std::to_string(vertex.position[1]) + ' ' + std::to_string(vertex.position[2]) + '\n';
	}
	for (const auto& vertex : this->vertices) {
		out += "vn " + std::to_string(vertex.normal[0]) + ' ' + std::to_string(vertex.normal[1]) + ' ' + std::to_string(vertex.normal[2]) + '\n';
	}
	for (const auto& vertex : this->vertices) {
		out += "vt " + std::to_string(vertex.uv1[0]) + ' ' + std::to_string(vertex.uv1[1]) + '\n';
	}
	for (int i = 1; i <= this->vertices.size(); i += 3) {
		out += "f "
		       + std::to_string(i + 0) + '/' + std::to_string(i + 0) + '/' + std::to_string(i + 0) + ' '
		       + std::to_string(i + 1) + '/' + std::to_string(i + 1) + '/' + std::to_string(i + 1) + ' '
		       + std::to_string(i + 2) + '/' + std::to_string(i + 2) + '/' + std::to_string(i + 2) + '\n';
	}
	return out;
}
