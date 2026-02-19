#include "OSMDataFetcher.h"
#include "OSMDataParser.h"
#include "MeshManager.h"
#include "LogUtils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <iostream>

GLboolean glewExperimental = GL_TRUE;

int main() {

#ifdef NDEBUG
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::set_level(spdlog::level::trace);
#endif

    if (!glfwInit()) {
        spdlog::critical("Failed to initialize GLFW!");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1600, 1200, "3D Map", NULL, NULL);
    if (!window) {
        spdlog::critical("Failed to create GLFW window!");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        spdlog::critical("Failed to initialize GLEW!");
        glfwTerminate();
        return -1;
    }

    spdlog::debug("OpenGL Version: {}.", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    spdlog::debug("GLFW Version: {}.", glfwGetVersionString());
    spdlog::debug("GLEW Version : {}.", reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));

    std::string overpassUrl = "http://overpass-api.de/api/interpreter";
    double latMin, latMax, lonMin, lonMax;
    latMin = 54.8380;
    latMax = 54.8490;
    lonMin = 83.0880;
    lonMax = 83.1020;

    spdlog::trace("Input min latitude: {:0.6f}.", latMin);
    spdlog::trace("Input max latitude: {:0.6f}.", latMax);
    spdlog::trace("Input min longitude: {:0.6f}.", lonMin);
    spdlog::trace("Input max longitude: {:0.6f}.", lonMax);
    spdlog::trace("Input overpass URL: {}.", overpassUrl);

    spdlog::info("Fetching data from {}.", overpassUrl);
    OSMDataFetcher dataFetcher(latMin, latMax, lonMin, lonMax, overpassUrl);
    std::string jsonData = dataFetcher.FetchBuildings();

    spdlog::trace("JSON preview: {}...", jsonData.substr(0, 200));

    if (jsonData.find("ERROR") != std::string::npos) {
        spdlog::error("Not valid response from the server!");
        return 0;
    }

    if (jsonData.empty()) {
        spdlog::error("JSON is empty!");
        return 0;
    }

    spdlog::info("Parsing JSON contents.");
    auto mapData = ParseDataFromJson(jsonData);

    spdlog::debug("Received buildings: {}, received nodes: {}.", mapData->m_Ways.size(), mapData->m_Nodes.size());
    for (size_t i = 0; i < mapData->m_Nodes.size(); i++) {
        spdlog::trace("Node: id = {}, lat = {:.6f}, lon = {:.6f}", i, mapData->m_Nodes[i].lat, mapData->m_Nodes[i].lon);
    }
    for (size_t i = 0; i < mapData->m_Ways.size(); i++) {
        spdlog::trace("Way: id = {}, node ind = {}", i, vectorToString(mapData->m_Ways[i].nodeIndecies));
    }

    spdlog::info("Creating mesh data.");
    MeshManager meshManager;
    meshManager.CreateMeshes(*mapData);


    for (size_t i = 0; i < meshManager.m_BuildingMeshes.size(); i++) {
        spdlog::trace("Building: id = {}, coordinates = [{}].", i, vectorToString(meshManager.m_BuildingMeshes[i].vertices));
    }

    spdlog::info("Rendering the map.");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-900, 900, -1500, 1500, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (const auto& mesh : meshManager.m_BuildingMeshes) {
            glColor3f(mesh.color.r, mesh.color.g, mesh.color.b);

            glBegin(GL_POLYGON);
            for (const auto& vertex : mesh.vertices) {
                glVertex2f(vertex.x, vertex.z);
            }
            glEnd();

            glColor3f(0, 0, 0);
            glBegin(GL_LINE_LOOP);
            for (const auto& vertex : mesh.vertices) {
                glVertex2f(vertex.x, vertex.z);
            }
            glEnd();
        }

        glColor3f(1.0, 0, 0);
        glBegin(GL_LINE_LOOP);
            glVertex2f(meshManager.CoordinateTransform(latMin, lonMin).x, meshManager.CoordinateTransform(latMin, lonMin).z);
            glVertex2f(meshManager.CoordinateTransform(latMin, lonMax).x, meshManager.CoordinateTransform(latMin, lonMax).z);
            glVertex2f(meshManager.CoordinateTransform(latMax, lonMax).x, meshManager.CoordinateTransform(latMax, lonMax).z);
            glVertex2f(meshManager.CoordinateTransform(latMax, lonMin).x, meshManager.CoordinateTransform(latMax, lonMin).z);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glfwTerminate();
    return 0;
}