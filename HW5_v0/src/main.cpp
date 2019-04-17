//
//  main.cpp
//  HW5_Camera
//
//  Created by hansen on 2019/4/10.
//  Copyright © 2019 hansen. All rights reserved.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.hpp"
#include "Camera.h"
#include "ImGUI/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

// time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// control mode
bool isFPS = false;
int choice = 0;

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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

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
    bool show_demo_window = false;
    
    float camPos[3] = { 0.0f, 0.0f, 10.0f };
    float cubeCenter[3] = { -1.5f, 0.5f, -1.5f };
    float lookAtCenter[3] = {0.0f, 0.0f, 0.0f};
    float ortho[4] = { -8.0f,8.0f , -6.0f, 6.0f };
    float perspect[3] = {45.0f, SCR_WIDTH, SCR_HEIGHT};
    float near_far[2] = { 0.1f, 100.0f };

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Menu");
        ImGui::Text("Make a choice below.");
        
        if (ImGui::Button("Orthographic projection")){
            choice = 1;
        }
        if (ImGui::Button("Perspective projection")){
            choice = 2;
        }
        if (ImGui::Button("View Changing")){
            choice = 3;
        }
        if (ImGui::Button("FPS")){
            choice = 4;
        }
        
        switch (choice) {
            case 1:
                ImGui::Text("Input the parameters of glm::ortho(left,right,bottom,top,zNear,zFar).");
                ImGui::InputFloat4("Othro paras", ortho, 2);
                ImGui::InputFloat2("Near and Far", near_far, 2);
                break;
            case 2:
                ImGui::Text("Input the parameters of glm::(fovy,aspect,zNear,zFar).");
                ImGui::InputFloat3("Perspect paras", perspect, 2);
                ImGui::InputFloat2("Near and Far", near_far, 2);
                break;
            case 3:
                ImGui::Text("The initial radius is 10.0f.");
                break; 
            case 4:
                ImGui::Text("FPS mode.");
                isFPS = true;
            default:
                break;
        }
        if(choice == 1 || choice == 2){
            ImGui::InputFloat3("Camera position", camPos, 2);
            ImGui::InputFloat3("Look at center", lookAtCenter, 2);
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
        // default initialize as an identity matrix
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        
        // View changing
        float Radius = 10.0f;
        float camPosX = sin(glfwGetTime()) * Radius;
        float camPosZ = cos(glfwGetTime()) * Radius;
        
        switch (choice) {
            case 1:
                model = glm::translate(model, glm::vec3(cubeCenter[0], cubeCenter[1], cubeCenter[2]));
                view = glm::lookAt(
                           glm::vec3(camPos[0], camPos[1], camPos[2]),
                           glm::vec3(lookAtCenter[0], lookAtCenter[1], lookAtCenter[2]),
                           glm::vec3(0.0f, 1.0f, 0.0f)
                        );
                projection = glm::ortho(ortho[0], ortho[1], ortho[2], ortho[3], near_far[0], near_far[1]);
                break;
            case 2:
                model = glm::translate(model, glm::vec3(cubeCenter[0], cubeCenter[1], cubeCenter[2]));
                view = glm::lookAt(
                           glm::vec3(camPos[0], camPos[1], camPos[2]),
                           glm::vec3(lookAtCenter[0], lookAtCenter[1], lookAtCenter[2]),
                           glm::vec3(0.0f, 1.0f, 0.0f)
                        );
                projection = glm::perspective(glm::radians(perspect[0]),perspect[1] / perspect[2],near_far[0],near_far[1]);
                break;
            case 3:
                view = glm::lookAt(
                           glm::vec3(camPosX, 0.0f, camPosZ),
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::vec3(0.0f, 1.0f, 0.0f)
                        );
                projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                break;
            case 4:
                if(isFPS){
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                view = camera.GetViewMatrix();
                projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                break;
            default:
                break;
        }
        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view",glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isFPS = false;
        choice = 0;
    }
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT,deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT,deltaTime);
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset,yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
