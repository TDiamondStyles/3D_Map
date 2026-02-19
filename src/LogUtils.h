#pragma once

#include <glm/glm.hpp>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ostream>

#define LOG_INFO_FUNC_ON_ENTER() spdlog::debug("Entered [{}].", __FUNCTION__)

template<typename T>
std::string vectorToString(const std::vector<T>& vec, const std::string& delimiter = ", ") {
    if (vec.empty()) return "";
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);

    for (size_t i = 0; i < vec.size(); ++i) {
        oss << std::fixed << std::setprecision(6) << vec[i];
        if (i != vec.size() - 1) oss << delimiter;
    }
    return oss.str();
}

inline std::string vectorToString(const std::vector<glm::vec3>& vec, const std::string& delimiter = ", ") {
    if (vec.empty()) return "";

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);

    for (size_t i = 0; i < vec.size(); ++i) {
        oss << "(" << vec[i].x << ", " << vec[i].y << ", " << vec[i].z << ")";
        if (i != vec.size() - 1) oss << delimiter;
    }
    return oss.str();
}