#pragma once

#include <vector>

struct Node {
	double lat;
	double lon;
};

struct Way {
	std::vector<size_t> nodeIndidcies;
};

struct MapData {
	std::vector<Node> m_Nodes;
	std::vector<Way> m_Ways;
};