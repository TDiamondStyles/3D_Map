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
    src/MeshManager.h)

target_compile_definitions(3D_Map PRIVATE GLEW_STATIC)

target_include_directories(3D_Map PRIVATE
    dependencies/GLFW/include  
    dependencies/GLEW/include
    vendors/httplib
    vendors/nlohmann
    vendors/glm
)



target_link_libraries(3D_Map PRIVATE
    dependencies/GLFW/lib-vc2022/glfw3
    dependencies/GLEW/lib/Release/x64/glew32s
    opengl32
)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})