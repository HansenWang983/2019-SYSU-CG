//
//  main.cpp
//  HW7_Shadowing_Mapping
//
//  Created by hansen on 2019/5/9.
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

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderScene(Shader &shader);
void RenderQuad();

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 15.0f, 15.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -45.0f);
float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;

// time
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

// Global variable
bool isLightMoving = false;
bool isFPS = false;
int choice = 0;
GLuint planeVAO, cubeVAO, cubeVBO;

// lighting
glm::vec3 lightPos(5.0f, 10.0f, 5.0f);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shadowing_Mapping", NULL, NULL);
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
    Shader lightShader = Shader("Shader/light.vs", "Shader/light.fs");
    Shader simpleDepthShader = Shader("Shader/shadow_mapping_depth.vs","Shader/shadow_mapping_depth.fs");
    Shader debugDepthQuad = Shader("Shader/debug_quad.vs", "Shader/debug_quad.fs");
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);
    
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
    
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
    
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
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    
    // only position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    //  bind the plane 
    float planeVertices[] = {
        // Positions            // Normals       
        25.0f, -2.0f, 25.0f,    0.0f, 1.0f, 0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f, 
        -25.0f, -2.0f, 25.0f,   0.0f, 1.0f, 0.0f,

        25.0f, -2.0f, 25.0f,    0.0f, 1.0f, 0.0f,
        25.0f, -2.0f, -25.0f,   0.0f, 1.0f, 0.0f,
        -25.0f, -2.0f, -25.0f,  0.0f, 1.0f, 0.0f
    };
    // Setup plane VAO
    GLuint planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Configure depth map FBO
    const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // - Create depth texture
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // shader configuration
    // --------------------
    phongShader.use();
//    phongShader.setInt("diffuseTexture", 0);
    phongShader.setInt("shadowMap", 1);
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);
    
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
        
        ImGui::RadioButton("Orthographic Shadowing", &choice, 1);
        ImGui::RadioButton("Projection Shadowing", &choice, 2);
        
        if(choice == 1 || choice == 2){
            if (ImGui::CollapsingHeader("Lighting Options")) {
                
                ImGui::SliderFloat("lightColor.x", &lightColor.x, 0.0f, 1.0f, "X = %.1f");
                ImGui::SliderFloat("lightColor.y", &lightColor.y, 0.0f, 1.0f, "Y = %.1f");
                ImGui::SliderFloat("lightColor.z", &lightColor.z, 0.0f, 1.0f, "Z = %.1f");
                
                ImGui::SliderFloat("ambientStrength", &light.ambient, 0.0f, 1.0f, "ambientStrength = %.1f");
                ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f, "specularStrength = %.1f");
                ImGui::SliderFloat("shininess", &shininess, 0.0f, 256.0f, "shininess= %.0f");
                
                ImGui::SliderFloat("lightPos.x", &lightPos.x, -10.0f, 20.0f, "X = %.1f");
                ImGui::SliderFloat("lightPos.y", &lightPos.y, -10.0f, 20.0f, "Y = %.1f");
                ImGui::SliderFloat("lightPos.z", &lightPos.z, -10.0f, 20.0f, "Z = %.1f");
            }
        }
        
        ImGui::End();
        
        // clear the colorbuffer
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        // also clear the depth buffer now!
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // unbind VAO every time
        glBindVertexArray(0);
        
        // render

        // -------------------------------------------
        // 1. Render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_FRONT);
        
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = 1.0f, far_plane = 30.0f;
        if (choice == 1) {
            // Orthographic 
            lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        }
        else if(choice == 2){
            // Projection
           lightProjection = glm::perspective(30.0f, (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        }
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        
        // - now render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        RenderScene(simpleDepthShader);
        
//        glCullFace(GL_BACK);
//        glDisable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 view = currentView;
        glm::mat4 projection = currentProjection;
                
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
        
        // 2. render scene as normal using the generated depth/shadow map
        // --------------------------------------------------------------
        phongShader.use();
        // set Camera transform
        phongShader.setMat4("projection", glm::value_ptr(projection));
        phongShader.setMat4("view", glm::value_ptr(view));
        
        // set Light paras
        phongShader.setFloat("ambientStrength", light.ambient);
        phongShader.setFloat("specularStrength", specularStrength);
        phongShader.setFloat("shininess", shininess);
        phongShader.setFloat3("viewPos", glm::value_ptr(camera.Position));
        phongShader.setFloat3("lightPos", glm::value_ptr(lightPos));
        phongShader.setFloat3("lightColor", glm::value_ptr(lightColor));
        
        // set Light transform
        phongShader.setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMatrix));
        glActiveTexture(GL_TEXTURE0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // set transform paras of floor and cube and render
        RenderScene(phongShader);

        // 3. DEBUG: visualize depth map by rendering it to plane
        debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // RenderQuad();


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
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &cubeVBO);
    
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
void RenderScene(Shader &shader)
{
    // Floor
    glm::mat4 model;
    shader.setMat4("model", glm::value_ptr(model));
    shader.setFloat3("objectColor", glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
    glBindVertexArray(planeVAO);    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    // Cube1
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(0.0f, 5.5f, 0.0f));
    model = glm::rotate(model, 60.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
    shader.setMat4("model", glm::value_ptr(model));
    shader.setFloat3("objectColor", glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    // Cube2
    model = glm::mat4();
    model = glm::translate(model, glm::vec3(-5.0f, -1.0f, 3.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", glm::value_ptr(model));
    shader.setFloat3("objectColor", glm::value_ptr(glm::vec3(1.0f, 0.5f, 0.31f)));
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

// RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets
// and post-processing effects.
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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
