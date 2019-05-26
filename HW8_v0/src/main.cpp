//
//  main.cpp
//  HW8_Bezier_Curve
//
//  Created by hansen on 2019/5/22.
//  Copyright © 2019 hansen. All rights reserved.
//

// system headers
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

// third-party headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// local headers
#include "Shader.hpp"
#include "ImGUI/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
vector<glm::vec3>::iterator findNearestControlPoint(const float xpos, const float ypos, const float threshold);
vector<GLfloat> controlPoints2floatVector(vector<glm::vec3> controlPoints);
glm::vec3 glfwPos2ndcPos(const glm::vec3 point);
void draw_process(vector<glm::vec3> points, float t);
void renderPointsLines(Shader &shader,const vector<GLfloat> &nodeData, float pointSize);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Global variable
vector<glm::vec3> controlPoints;
vector<glm::vec3>::iterator currPointIter;
bool isLeftButtonPressed = false;
bool isDrawing = false;

GLuint pointVAO, pointVBO;
Shader pointShader;

int main(int argc, const char * argv[]) {
    // glfw: initialize and configure
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;
    
    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Bezier Curve", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // load the shader program
    pointShader = Shader("Shader/point.vs","Shader/point.fs");
    Shader curveShader = Shader("Shader/curve.vs","Shader/curve.fs");
    
    // vertex data
    float step = 0.001;
    vector<float> data;
    data.resize(int(1 / step));
    for (int i = 0; i < data.size(); ++i) {
        data[i] = i * step;
    }
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
    
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // control variables in render loop
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
    float current_t = 0.0;
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        
        // clear the colorbuffer
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        // unbind VAO every time
        glBindVertexArray(0);
        
        // Render Control Points
        if (isLeftButtonPressed) {
            // record the selected point index
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            // check if can selected the nearest control point
            currPointIter = findNearestControlPoint(xpos, ypos, 100);
            if (currPointIter != controlPoints.end()){
                // update position
                *currPointIter = glm::vec3(xpos, ypos, 0.0f);
            }
        }
        
        auto pointdata = controlPoints2floatVector(controlPoints);
        renderPointsLines(pointShader, pointdata, 5.0f);
        
        // Render Beizer Curve
        curveShader.use();
        curveShader.setInt("size", int(controlPoints.size()));
        for (auto index = 0; index < controlPoints.size(); index++){
            curveShader.setFloat3(("points[" + std::to_string(index) + "]").c_str(),glm::value_ptr(glfwPos2ndcPos(controlPoints[index])));
        }
        if(!controlPoints.empty()){
            glBindVertexArray(VAO);
            glPointSize(2.0f);
            glDrawArrays(GL_POINTS, 0, int(data.size()));
            glBindVertexArray(0);
        }
        
        // dynamic draw
        if(isDrawing){
            // deactivate the mouse movement
            glfwSetMouseButtonCallback(window, NULL);
            if (current_t <= 1.0) {
                current_t += 0.0005;
                draw_process(controlPoints, current_t);
            }
        } else {
            current_t = 0.0;
            // reactivate the mouse movement
            glfwSetMouseButtonCallback(window, mouse_button_callback);
        }
        
        // render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        isDrawing = !isDrawing;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // add one point on the canvas  && move the selected points
        if (action == GLFW_PRESS) {
            isLeftButtonPressed = true;
            if (findNearestControlPoint(xpos, ypos, 100) == controlPoints.end()) {
                // add the selected point
                controlPoints.push_back(glm::vec3(xpos, ypos, 0.0f));
            }
        }
        
        if (action == GLFW_RELEASE) {
            currPointIter = controlPoints.end();
            isLeftButtonPressed = false;
        }
    }
    
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // delete the last control point on the canvas
        if(!controlPoints.empty()){
           controlPoints.pop_back();
        }
    }
}

vector<glm::vec3>::iterator findNearestControlPoint(const float xpos, const float ypos, const float threshold) {
    // 粗略查找点击范围中是否有可以控制的点
    vector<glm::vec3>::iterator res = controlPoints.end();
    
    auto dist = [&xpos, &ypos](const vector<glm::vec3>::iterator iter) -> float {
        return pow((xpos - iter->x), 2) + pow((ypos - iter->y), 2);
    };
    
    for (auto iter = controlPoints.begin(); iter != controlPoints.end(); ++iter) {
        auto dis = dist(iter);
        if (dis < threshold) {
            if (res == controlPoints.end()) {
                res = iter;
                
            }
            else {
                res = (dist(res) < dis) ? res : iter;
                
            }
        }
    }
    
    return res;
}

vector<GLfloat> controlPoints2floatVector(vector<glm::vec3> cp){
    vector<GLfloat> res;
    res.clear();
    for (auto iter = cp.begin(); iter != cp.end(); iter++){
        res.push_back(iter->x);
        res.push_back(iter->y);
        res.push_back(iter->z);
    }
    // normalize to [-1, 1]
    for (unsigned int i = 0; i < res.size(); i = i + 3) {
        auto norx = (2 * res[i]) / SCR_WIDTH - 1;
        auto nory = 1 - (2 * res[i + 1]) / SCR_HEIGHT;
        res[i] = norx;
        res[i + 1] = nory;
    }
    return res;
}

glm::vec3 glfwPos2ndcPos(const glm::vec3 point){
    glm::vec3 res;
    res.x = (2 * point.x) / SCR_WIDTH - 1;
    res.y = 1 - (2 * point.y) / SCR_HEIGHT;
    res.z = 0.0f;
    return res;
}

void draw_process(vector<glm::vec3> points, float t){
    if (points.size() < 2){
        return;
    }
    vector<glm::vec3> next_nodes;
    for (int i = 0; i < points.size() - 1; ++i)
    {
        float x = (1 - t) * points[i].x + t * points[i + 1].x;
        float y = (1 - t) * points[i].y + t * points[i + 1].y;
        next_nodes.push_back(glm::vec3(x, y, 0.0f));
    }
    auto nodeData = controlPoints2floatVector(next_nodes);
    renderPointsLines(pointShader, nodeData, 3.5f);
    draw_process(next_nodes, t);
}

void renderPointsLines(Shader &shader,const vector<GLfloat> &nodeData, float pointSize){
    shader.use();
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER,pointVBO);
    
    glBufferData(GL_ARRAY_BUFFER, nodeData.size() * sizeof(GLfloat), nodeData.data(), GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glPointSize(pointSize);
    glDrawArrays(GL_POINTS, 0, int(nodeData.size() / 3));
    
    // Render lines between control points
    glLineWidth(1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, int(nodeData.size() / 3));
    glBindVertexArray(0);
}
