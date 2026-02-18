#pragma once

#include "MapDataTypes.h"

#include <glm/glm.hpp>

#include <vector>

struct BuildingMesh {
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices;
	glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
};

class MeshManager {
public:
	glm::vec3 CoordinateTransform(double lat, double lon);
	void CreateMeshes(const MapData& mapData);

	std::vector<BuildingMesh> m_BuildingMeshes;

private:
	double m_LatRef, m_LonRef;
};