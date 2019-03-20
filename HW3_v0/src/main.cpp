//
//  main.cpp
//  HW3_Draw_a_line
//
//  Created by hansen on 2019/3/20.
//  Copyright © 2019 hansen. All rights reserved.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "ImGUI/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Utilities.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Draw Line", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // load the shader program
    Shader shader = Shader("Shader/basic.vs", "Shader/basic.fs");
    
    GLuint VAO[2],VBO[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    // three 2D integer points
    float vertices1[]={
        -60.0,-80.0,
        160.0,140.0,
        160.0,-80.0
    };
    
    // draw a triangle with Bresenham's algorithm
    auto data = genTrianglePositions(
        Point(vertices1[0],vertices1[1]),
        Point(vertices1[2],vertices1[3]),
        Point(vertices1[4],vertices1[5])
    );

    // convert screen coordiante to gl coordinate
    for (int i = 0; i < data.size(); i = i + 3) {
        data[i] = 2 * data[i] / SCR_WIDTH;
        data[i + 1] = 2 * data[i + 1] / SCR_HEIGHT;
    }

    // bind the VAO, VBO with points
    PointsBindVAO(VAO[0],VBO[0],data);

    // for debugging
    // cout << data[0] <<endl;
    // cout << data[1] <<endl;
    // cout << data[3] <<endl;
    // cout << data[4] <<endl;
    // cout << data[6] <<endl;
    // cout << data[7] <<endl;
    // cout << data[9] <<endl;
    // cout << data[10] <<endl;
    // cout<<data.size()<<endl;

    // draw the circle with Bresenham's algorithm
    // centre 
    Point centre(0.0f,0.0f);
    // radius
    int radius = 200;

    auto circleData = genCirclePositions(centre,radius);
    // convert screen coordiante to gl coordinate
    for (int i = 0; i < circleData.size(); i = i + 3) {
        circleData[i] = 2 * circleData[i] / SCR_WIDTH;
        circleData[i + 1] = 2 * circleData[i + 1] / SCR_HEIGHT;
    }
    // bind the VAO, VBO with points
    PointsBindVAO(VAO[1],VBO[1],circleData);

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
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    int choice = 0;

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
        
        ImGui::Begin("Menu");
        ImGui::Text("Make a choice below.");
        
        if (ImGui::Button("Draw triangle")){
            choice = 1;
        }
        if (ImGui::Button("Draw circle")){
            choice = 2;
        }

        switch (choice) {
            case 1:
                ImGui::Text("A triangle will be drawn.");
                break;
            case 2:
                ImGui::Text("A circle will be drawn.");
                break;
            default:
                break;
        }
        ImGui::End();
    
        // clear the colorbuffer
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        // unbind VAO every time
        glBindVertexArray(0);
        
        // be sure to activate the shader
        shader.use();

        switch (choice) {
            case 1:
                glBindVertexArray(VAO[0]);
                glDrawArrays(GL_POINTS, 0, int(data.size()) / 3);
                break;
            case 2:
                glBindVertexArray(VAO[1]);
                glDrawArrays(GL_POINTS, 0, int(circleData.size()) / 3);
                break;
            default:
                break;
        }

        // render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
