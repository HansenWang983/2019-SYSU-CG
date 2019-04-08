//
//  main.cpp
//  HW4_Transformation
//
//  Created by hansen on 2019/4/4.
//  Copyright © 2019 hansen. All rights reserved.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "ImGUI/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

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
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
//    glDisable(GL_DEPTH_TEST);
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // six 3D vertices
    float vertices1[] = {
        // 背面
        -2.0f, -2.0f, -2.0f,  0.8f, 0.2f, 0.2f,
        2.0f, -2.0f, -2.0f,   0.8f, 0.2f, 0.2f,
        2.0f,  2.0f, -2.0f,   0.8f, 0.2f, 0.2f,
        2.0f,  2.0f, -2.0f,   0.8f, 0.2f, 0.2f,
        -2.0f,  2.0f, -2.0f,  0.8f, 0.2f, 0.2f,
        -2.0f, -2.0f, -2.0f,  0.8f, 0.2f, 0.2f,
        // 左
        -2.0f,  2.0f,  2.0f,  0.2f, 0.8f, 0.2f,
        -2.0f,  2.0f, -2.0f,  0.2f, 0.8f, 0.2f,
        -2.0f, -2.0f, -2.0f,  0.2f, 0.8f, 0.2f,
        -2.0f, -2.0f, -2.0f,  0.2f, 0.8f, 0.2f,
        -2.0f, -2.0f,  2.0f,  0.2f, 0.8f, 0.2f,
        -2.0f,  2.0f,  2.0f,  0.2f, 0.8f, 0.2f,
        // 正面
        -2.0f, -2.0f,  2.0f,  0.2f, 0.2f, 0.8f,
        2.0f, -2.0f,  2.0f,   0.2f, 0.2f, 0.8f,
        2.0f,  2.0f,  2.0f,   0.2f, 0.2f, 0.8f,
        2.0f,  2.0f,  2.0f,   0.2f, 0.2f, 0.8f,
        -2.0f,  2.0f,  2.0f,  0.2f, 0.2f, 0.8f,
        -2.0f, -2.0f,  2.0f,  0.2f, 0.2f, 0.8f,
        // 右
        2.0f,  2.0f,  2.0f,   0.2f, 0.8f, 0.8f,
        2.0f,  2.0f, -2.0f,   0.2f, 0.8f, 0.8f,
        2.0f, -2.0f, -2.0f,   0.2f, 0.8f, 0.8f,
        2.0f, -2.0f, -2.0f,   0.2f, 0.8f, 0.8f,
        2.0f, -2.0f,  2.0f,   0.2f, 0.8f, 0.8f,
        2.0f,  2.0f,  2.0f,   0.2f, 0.8f, 0.8f,
        // 下
        -2.0f, -2.0f, -2.0f,  0.8f, 0.2f, 0.8f,
        2.0f, -2.0f, -2.0f,   0.8f, 0.2f, 0.8f,
        2.0f, -2.0f,  2.0f,   0.8f, 0.2f, 0.8f,
        2.0f, -2.0f,  2.0f,   0.8f, 0.2f, 0.8f,
        -2.0f, -2.0f,  2.0f,  0.8f, 0.2f, 0.8f,
        -2.0f, -2.0f, -2.0f,  0.8f, 0.2f, 0.8f,
        // 上
        -2.0f,  2.0f, -2.0f,  0.8f, 0.8f, 0.2f,
        2.0f,  2.0f, -2.0f,   0.8f, 0.8f, 0.2f,
        2.0f,  2.0f,  2.0f,   0.8f, 0.8f, 0.2f,
        2.0f,  2.0f,  2.0f,   0.8f, 0.8f, 0.2f,
        -2.0f,  2.0f,  2.0f,  0.8f, 0.8f, 0.2f,
        -2.0f,  2.0f, -2.0f,  0.8f, 0.8f, 0.2f
    };
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
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
    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    int choice = 0;
    bool show_demo_window = false;
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
        
        if (ImGui::Button("Draw a static cube")){
            choice = 1;
        }
        if (ImGui::Button("Translation")){
            choice = 2;
        }
        if (ImGui::Button("Rotation")){
            choice = 3;
        }
        if (ImGui::Button("Scaling")){
            choice = 4;
        }
        if (ImGui::Button("Solar System")){
            choice = 5;
        }
        
        switch (choice) {
            case 1:
                ImGui::Text("A static cube will be shown.");
                break;
            case 2:
                ImGui::Text("A cube wiil translate horizontally and continuously.");
                break;
            case 3:
                ImGui::Text("A cube will rotate among the x=z axis in XoZ plane continuously.");
                break; 
            case 4:
                ImGui::Text("A cube will scale continuously.");
                break;
            case 5:
                ImGui::Text("Sun and Earth.");
                break;
            default:
                break;
        }
        ImGui::End();
       
        // clear the colorbuffer
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // also clear the depth buffer now!
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // unbind VAO every time
        glBindVertexArray(0);
        // be sure to activate the shader
        shader.use();
        
        // use glm for matrix transformation
        glm::mat4 model; // default initialize as an identity matrix
        glm::mat4 view;
        glm::mat4 projection;
        
        // Bonus
        glm::mat4 solar_model;
        glm::mat4 earth_model;

        // Camera matrix
        view = glm::lookAt(
            glm::vec3(10.0f, 10.0f, 10.0f), // the position of your camera, in world space
            glm::vec3(0.0f, 0.0f, 0.0f), // where you want to look at, in world space
            glm::vec3(0.0f, 1.0f, 0.0f)  // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too
        );

        // Projection matrix 
        projection = glm::perspective(
            45.0f, // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90&deg; (extra wide) and 30&deg; (quite zoomed in)
            (float)SCR_WIDTH / (float)SCR_HEIGHT, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960
            0.1f,  // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            100.0f  // Far clipping plane. Keep as little as possible.
        );

        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));
        
        switch (choice) {
            case 1:
                shader.setMat4("model", glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            case 2:
                model = glm::translate(model, (float)(5 * sin(glfwGetTime())) * glm::vec3(2.0f,0.0f,0.0f));
                shader.setMat4("model", glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            case 3:
                model = glm::rotate(model, glm::radians((float)glfwGetTime() * 100.0f), glm::vec3(1.0f, 0.0f, 1.0f));
                shader.setMat4("model", glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            case 4:
                model = glm::scale(model, (float)abs(sin(glfwGetTime())) * glm::vec3(2.0f, 2.0f, 2.0f));
                shader.setMat4("model", glm::value_ptr(model));
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            case 5:
                // Rotation
                solar_model = glm::rotate(solar_model, glm::radians((float)glfwGetTime() * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

                // smaller than sun
                earth_model = glm::scale(earth_model, glm::vec3(0.5f, 0.5f, 0.5f));
                // Rotation, faster than sun
                earth_model = glm::rotate(earth_model, glm::radians((float)glfwGetTime() * 100.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                // // Revolution
                earth_model = glm::translate(earth_model, glm::vec3(10.0f, 0.0f, 10.0f));

                glBindVertexArray(VAO);
                shader.setMat4("model", glm::value_ptr(solar_model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                shader.setMat4("model", glm::value_ptr(earth_model));
                glDrawArrays(GL_TRIANGLES, 0, 36);
                break;
            default:
                break;
        }
        
        // Imgui render
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
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
