//
//  Shader.hpp
//  HW7_Shadowing_Mapping
//
//  Created by hansen on 2019/5/9.
//  Copyright © 2019 hansen. All rights reserved.
//


#ifndef Shader_hpp
#define Shader_hpp

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

class Shader {
public:
    // the program ID
    GLuint shaderProgramID;
    
    // default constructor
    Shader() {}
    // constructor reads and builds the shader
    Shader(const GLchar* vsPath, const GLchar* fsPath);
    
    // use/activate the shader
    void use();
    
    // utility uniform functions
    void setBool(const string &name, bool value) const;
    void setInt(const string &name, int value) const;
    void setFloat(const string &name, float value) const;
    void setFloat3(const string &name, const float vec[]) const;
    void setFloat4(const string &name, const float vec[]) const;
    
    // for mat4 uniform 
    void setMat4(const string &name, const float vec[]) const;
};

#endif /* Shader_hpp */
