#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OSMDataFetcher.h"
#include "OSMDataParser.h"
#include "MeshManager.h"

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Map", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "GLFW Version: " << glfwGetVersionString() << "\n";
    std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) << "\n";

    double latMin, latMax, lonMin, lonMax;
    latMin = 55.7490;
    latMax = 55.7545;
    lonMin = 37.6160;
    lonMax = 37.6235;

    OSMDataFetcher dataFetcher(latMin, latMax, lonMin, lonMax, "http://overpass-api.de/api/interpreter");

    std::cout << "Fetching data from overpass-api.de" << std::endl;

    std::string jsonData = dataFetcher.FetchBuildings();
    std::cout << jsonData << '\n';

    if (jsonData.find("ERROR") != std::string::npos) {
        std::cin.get();
        return 0;
    }

    auto mapData = ParseDataFromJson(jsonData);

    std::cout << "Recieved " << mapData->m_Ways.size() << " buildings and " << mapData->m_Nodes.size() << " nodes." << std::endl;

    for (size_t i = 0; i < mapData->m_Nodes.size(); i++) {
        std::cout << std::setprecision(15) << i << " " << mapData->m_Nodes[i].lat << " " << mapData->m_Nodes[i].lon << "\n";
    }

    for (const auto& way : mapData->m_Ways) {
        for (auto i : way.nodeIndidcies)
            std::cout << i << ' ';
        
        std::cout << '\n';
    }

    MeshManager meshManager;
    meshManager.CreateMeshes(*mapData);


    for (auto& building : meshManager.m_BuildingMeshes) {
        for (auto& corner : building.vertices) {
            std::cout << corner.x << ' ' << corner.y << ' ' << corner.z << '\n';
        }

        std::cout << '\n';
    }


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500, 500, -500, 500, -1, 1);
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