cmake_minimum_required(VERSION 3.15)
project(3D_Map)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(3D_Map 
    src/Application.cpp
    src/OSMDataFetcher.cpp
    src/OSMDataFetcher.h
    src/OSMDataParser.cpp
    src/OSMDataParser.h
    src/MapDataTypes.h
    src/MeshManager.cpp
    src/MeshManager.h
    src/LogUtils.h
)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(curl REQUIRED)
find_package(spdlog REQUIRED)

target_link_libraries(3D_Map PRIVATE
    glfw
    GLEW::GLEW
    glm::glm
    nlohmann_json::nlohmann_json
    CURL::libcurl
    spdlog::spdlog
    opengl32
)

target_compile_definitions(3D_Map PRIVATE GLEW_STATIC)