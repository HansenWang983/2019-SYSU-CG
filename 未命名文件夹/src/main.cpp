//
//  main.cpp
//  HW6_Lights_and_Shading
//
//  Created by hansen on 2019/4/16.
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
Camera camera(glm::vec3(0.0f, 5.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f);
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

// time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// control mode
bool isLightMoving = false;
bool isFPS = false;
int choice = 0;


// lighting
glm::vec3 lightPos(0.0f, 0.0f, 5.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

struct Light {
    float ambient;
    float diffuse;
    float specular;
    Light(float amb = 0.1f, float dif = 0.5f,float spe = 1.0f){
        ambient = amb;
        diffuse = dif;
        specular = spe;
    }
};

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lights_and_Shading", NULL, NULL);
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
    Shader phongShader = Shader("Shader/Phong.vs", "Shader/Phong.fs");
    Shader gouraudShader = Shader("Shader/Gouraud.vs", "Shader/Gouraud.fs");
    Shader lightShader = Shader("Shader/light.vs", "Shader/light.fs");
    Shader materialShader = Shader("Shader/materials.vs", "Shader/materials.fs");
    Shader objectShader;
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
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f, -1.0f,
        2.0f, -2.0f, -2.0f,  0.0f, 0.0f, -1.0f,
        2.0f,  2.0f, -2.0f,  0.0f, 0.0f, -1.0f,
        2.0f,  2.0f, -2.0f,  0.0f, 0.0f, -1.0f,
        -2.0f,  2.0f, -2.0f, 0.0f, 0.0f, -1.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, 0.0f, -1.0f,
        // 左
        -2.0f,  2.0f,  2.0f, -1.0f,  0.0f, 0.0f,
        -2.0f,  2.0f, -2.0f, -1.0f,  0.0f, 0.0f,
        -2.0f, -2.0f, -2.0f, -1.0f,  0.0f, 0.0f,
        -2.0f, -2.0f, -2.0f, -1.0f,  0.0f, 0.0f,
        -2.0f, -2.0f,  2.0f, -1.0f,  0.0f, 0.0f,
        -2.0f,  2.0f,  2.0f, -1.0f,  0.0f, 0.0f,
        // 正面
        -2.0f, -2.0f,  2.0f, 0.0f,  0.0f,  1.0f,
        2.0f, -2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        2.0f,  2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        2.0f,  2.0f,  2.0f,  0.0f,  0.0f,  1.0f,
        -2.0f,  2.0f,  2.0f, 0.0f,  0.0f,  1.0f,
        -2.0f, -2.0f,  2.0f, 0.0f,  0.0f,  1.0f,
        // 右
        2.0f,  2.0f,  2.0f, 1.0f,  0.0f,  0.0f,
        2.0f,  2.0f, -2.0f, 1.0f,  0.0f,  0.0f,
        2.0f, -2.0f, -2.0f, 1.0f,  0.0f,  0.0f,
        2.0f, -2.0f, -2.0f, 1.0f,  0.0f,  0.0f,
        2.0f, -2.0f,  2.0f, 1.0f,  0.0f,  0.0f,
        2.0f,  2.0f,  2.0f, 1.0f,  0.0f,  0.0f,
        // 下
        -2.0f, -2.0f, -2.0f, 0.0f, -1.0f,  0.0f,
        2.0f, -2.0f, -2.0f,  0.0f, -1.0f,  0.0f,
        2.0f, -2.0f,  2.0f,  0.0f, -1.0f,  0.0f,
        2.0f, -2.0f,  2.0f,  0.0f, -1.0f,  0.0f,
        -2.0f, -2.0f,  2.0f, 0.0f, -1.0f,  0.0f,
        -2.0f, -2.0f, -2.0f, 0.0f, -1.0f,  0.0f,
        // 上
        -2.0f,  2.0f, -2.0f, 0.0f,  1.0f,  0.0f,
        2.0f,  2.0f, -2.0f,  0.0f,  1.0f,  0.0f,
        2.0f,  2.0f,  2.0f,  0.0f,  1.0f,  0.0f,
        2.0f,  2.0f,  2.0f,  0.0f,  1.0f,  0.0f,
        -2.0f,  2.0f,  2.0f, 0.0f,  1.0f,  0.0f,
        -2.0f,  2.0f, -2.0f, 0.0f,  1.0f,  0.0f,
    };
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // bind light object with a VAO, and VBO stays the same as cube, as it has the same vertices data.
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // only position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
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
    
    // record current MVP matrix in FPS mode
    glm::mat4 currentView = glm::lookAt(
                                        glm::vec3(0.0f, 10.0f, 10.0f),
                                        glm::vec3(0.0f, 0.0f, 0.0f),
                                        glm::vec3(0.0f, 1.0f, 0.0f)
                                        );
    glm::mat4 currentProjection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    
    // light paras
    Light light;
    float specularStrength = 0.5f;
    float shininess = 32;
    
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
        
        ImGui::Checkbox("Lamp Moving", &isLightMoving);
        ImGui::Checkbox("FPS", &isFPS);
        
        ImGui::RadioButton("Phong Shading", &choice, 1);
        ImGui::RadioButton("Gouraud Shading", &choice, 2);
        ImGui::RadioButton("Material Shading", &choice, 3);
        
        
        if(choice == 1 || choice == 2){
            if (ImGui::CollapsingHeader("Lighting Options")) {
                
                ImGui::SliderFloat("lightColor.x", &lightColor.x, 0.0f, 1.0f, "X = %.1f");
                ImGui::SliderFloat("lightColor.y", &lightColor.y, 0.0f, 1.0f, "Y = %.1f");
                ImGui::SliderFloat("lightColor.z", &lightColor.z, 0.0f, 1.0f, "Z = %.1f");
                
                ImGui::SliderFloat("ambientStrength", &light.ambient, 0.0f, 1.0f, "ambientStrength = %.1f");
                ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f, "specularStrength = %.1f");
                ImGui::SliderFloat("shininess", &shininess, 0.0f, 256.0f, "shininess= %.0f");
                
                ImGui::SliderFloat("lightPos.x", &lightPos.x, -10.0f, 10.0f, "X = %.1f");
                ImGui::SliderFloat("lightPos.y", &lightPos.y, -10.0f, 10.0f, "Y = %.1f");
                ImGui::SliderFloat("lightPos.z", &lightPos.z, -10.0f, 10.0f, "Z = %.1f");
            }
        }
        if(choice == 3){
            
            if (ImGui::CollapsingHeader("Lighting Options")) {
                
                ImGui::SliderFloat("lightColor.x", &lightColor.x, 0.0f, 1.0f, "X = %.1f");
                ImGui::SliderFloat("lightColor.y", &lightColor.y, 0.0f, 1.0f, "Y = %.1f");
                ImGui::SliderFloat("lightColor.z", &lightColor.z, 0.0f, 1.0f, "Z = %.1f");
                
                ImGui::SliderFloat("light.ambient", &light.ambient, 0.0f, 1.0f, "light.ambient = %.1f");
                ImGui::SliderFloat("light.diffuse", &light.diffuse, 0.0f, 1.0f, "light.diffuse = %.1f");
                ImGui::SliderFloat("light.specular", &light.specular, 0.0f, 1.0f, "light.specular = %.1f");
                ImGui::SliderFloat("shininess", &shininess, 0.0f, 256.0f, "shininess= %.0f");
                
                ImGui::SliderFloat("lightPos.x", &lightPos.x, -10.0f, 10.0f, "X = %.1f");
                ImGui::SliderFloat("lightPos.y", &lightPos.y, -10.0f, 10.0f, "Y = %.1f");
                ImGui::SliderFloat("lightPos.z", &lightPos.z, -10.0f, 10.0f, "Z = %.1f");
            }
        }
        
        ImGui::End();
        
        // clear the colorbuffer
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // also clear the depth buffer now!
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // unbind VAO every time
        glBindVertexArray(0);
        
        // use glm for matrix transformation
        // configuration for camera
        glm::mat4 view;
        glm::mat4 projection;
        
        view = currentView;
        projection = currentProjection;
        
        // Lighting moving
        float Radius = 10.0f;
        float lampPosX = sin(glfwGetTime()) * Radius;
        float lampPosZ = cos(glfwGetTime()) * Radius;
        
        if(isLightMoving){
            lightPos.x = lampPosX;
            lightPos.z = lampPosZ;
        }
        
        if(isFPS){
            if(glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED){
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            view = camera.GetViewMatrix();
            currentView = view;
            projection = glm::perspective(glm::radians(camera.Zoom),(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            currentProjection = projection;
        }
        
        switch (choice) {
            case 1:
                objectShader = phongShader;
                break;
            case 2:
                objectShader = gouraudShader;
                break;
            case 3:
                objectShader = materialShader;
            default:
                break;
        }
        // be sure to activate the shader
        objectShader.use();
        
        // configure uniform variables
        if(choice == 1 || choice == 2){
            // set light paras
            objectShader.setFloat("ambientStrength", light.ambient);
            objectShader.setFloat("specularStrength", specularStrength);
            objectShader.setFloat("shininess", shininess);
            objectShader.setFloat3("viewPos", glm::value_ptr(camera.Position));
            objectShader.setFloat3("lightPos", glm::value_ptr(lightPos));
            objectShader.setFloat3("objectColor", glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
            objectShader.setFloat3("lightColor", glm::value_ptr(lightColor));
        }
        else{
            // set light paras
            objectShader.setFloat3("light.position", glm::value_ptr(lightPos));
            objectShader.setFloat3("viewPos", glm::value_ptr(camera.Position));
            
            // light properties
            glm::vec3 diffuseColor = lightColor * glm::vec3(light.diffuse); // decrease the influence
            glm::vec3 ambientColor = diffuseColor * glm::vec3(light.ambient); // low influence
            objectShader.setFloat3("light.ambient", glm::value_ptr(ambientColor));
            objectShader.setFloat3("light.diffuse", glm::value_ptr(diffuseColor));
            objectShader.setFloat3("light.specular", glm::value_ptr(glm::vec3(light.specular)));
            
            // material properties
            objectShader.setFloat3("material.ambient", glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
            objectShader.setFloat3("material.diffuse", glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
            objectShader.setFloat3("material.specular", glm::value_ptr(glm::vec3(specularStrength))); // specular lighting doesn't have full effect on this object's material
            objectShader.setFloat("material.shininess", shininess);
        }
        // set transform paras
        glm::mat4 object_model; // default initialize as an identity matrix
        objectShader.setMat4("model", glm::value_ptr(object_model));
        objectShader.setMat4("view",glm::value_ptr(view));
        objectShader.setMat4("projection", glm::value_ptr(projection));
        // render the object
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // be sure to activate the lightShader
        lightShader.use();
        // set transform paras
        glm::mat4 light_model;
        light_model = glm::translate(light_model, lightPos);
        light_model = glm::scale(light_model, glm::vec3(0.2f));
        lightShader.setMat4("model", glm::value_ptr(light_model));
        lightShader.setMat4("view",glm::value_ptr(view));
        lightShader.setMat4("projection", glm::value_ptr(projection));
        // render the lighting
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // render
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
    glDeleteVertexArrays(1, &lightVAO);
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
    
    if(!isFPS){
        return;
    }
    
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isFPS = false;
        firstMouse = true;
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
    if(!isFPS){
        return;
    }
    
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
    if(!isFPS){
        return;
    }
    camera.ProcessMouseScroll(yoffset);
}
