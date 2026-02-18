#pragma once

#include "MapDataTypes.h"

#include <nlohmann/json.hpp>

#include <string>
#include <memory>

std::unique_ptr<MapData> ParseDataFromJson(std::string& json);