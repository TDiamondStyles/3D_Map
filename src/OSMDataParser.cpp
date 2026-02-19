#include "OSMDataParser.h"

#include <unordered_map>
#include <iostream>

using json = nlohmann::json;

std::unique_ptr<MapData> ParseDataFromJson(std::string& jsonData)
{
	const json parsedData = json::parse(jsonData);
    const auto& elements = parsedData["elements"];

    size_t node_count = 0;
    size_t way_count = 0;

    for (const auto& elem : elements) {
        std::string type = elem["type"];
        if (type == "node") node_count++;
        else if (type == "way") way_count++;
    }

    auto map = std::make_unique<MapData>();
    map->m_Nodes.reserve(node_count);
    map->m_Ways.reserve(way_count);

    std::unordered_map<uint64_t, size_t> osmIdToInternalId;

    node_count = 0;
    for (const auto& elem : elements) {
        if (elem["type"] == "node") {
            map->m_Nodes.emplace_back(Node{ elem["lat"].get<double>(),
                                      elem["lon"].get<double>() });

            osmIdToInternalId[elem["id"].get<uint64_t>()] = node_count++;
        }
    }

    for (const auto& elem : elements) {
        if (elem["type"] == "way") {
            map->m_Ways.emplace_back();
            auto& way = map->m_Ways.back();

            for (const auto& node : elem["nodes"])
            {
                size_t id = osmIdToInternalId[node.get<uint64_t>()];
                way.nodeIndecies.emplace_back(id);
            }
        }
    }

    return map;
}
