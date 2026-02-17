#include "MeshManager.h"

void MeshManager::CreateMeshes(const MapData& mapData)
{
	m_BuildingMeshes.resize(mapData.m_Ways.size());
	m_LatRef = mapData.m_Nodes[0].lat;
	m_LonRef = mapData.m_Nodes[0].lon;

	for (size_t i = 0; i < mapData.m_Ways.size(); i++) {
		const auto& way = mapData.m_Ways[i];
		auto& building = m_BuildingMeshes[i];

		building.vertices.resize(way.nodeIndidcies.size() - 1); // last node index is the same as first so skip it
		for (size_t j = 0; j < way.nodeIndidcies.size() - 1; j++) { 
			const auto& node = mapData.m_Nodes[way.nodeIndidcies[j]];
			building.vertices[j] = CoordinateTransform(node.lat, node.lon);
		}
	}
}

glm::vec3 MeshManager::CoordinateTransform(double lat, double lon)
{
	constexpr double r = 6371000.0;
	constexpr double pi = 3.14159265358979323846f;
	constexpr double degToRad = pi / 180.0;
	double deltaLat = (lat - m_LatRef) * degToRad;
	double deltaLon = (lon - m_LonRef) * degToRad;

	return glm::vec3(r * deltaLon * glm::cos(m_LatRef * degToRad), 0, r * deltaLat);
}
