//
//  Shader.cpp
//  opengl_test
//
//  Created by hansen on 2019/3/7.
//  Copyright © 2019 hansen. All rights reserved.
//

#include "Shader.h"

Shader::Shader(const GLchar* vsPath, const GLchar* fsPath){
    
    // get the source file of GLSL
    string vsSource,fsSource;
    ifstream vsFile,fsFile;
    
    vsFile.exceptions(ifstream::failbit | ifstream::badbit);
    fsFile.exceptions(ifstream::failbit | ifstream::badbit);
    try{
        // open the source file
        vsFile.open(vsPath);
        fsFile.open(fsPath);
        
        // direct the buffer to stream
        stringstream vsStream,fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();
        // close the file 
        vsFile.close();
        fsFile.close();
        // cast to string
        vsSource = vsStream.str();
        fsSource = fsStream.str();
    }
    catch(ifstream::failure e){
        cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
    }
    
    const char* vsSource_C = vsSource.c_str();
    const char* fsSource_C = fsSource.c_str();
    // Compile a vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vsSource_C, NULL);
    glCompileShader(vertexShader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Compile a fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fsSource_C, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // link 2 shaders to shader program
    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);
//
//    // check for linking errors
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // delete the shader objects once we've linked them into the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use(){
    glUseProgram(shaderProgramID);
}

void Shader::setBool(const string &name, bool value) const
{         
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value); 
}

void Shader::setInt(const string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value); 
}

void Shader::setFloat(const string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value); 
} 

void Shader::setFloat4(const string &name, float vec[4]) const
{
    glUniform4f(glGetUniformLocation(shaderProgramID, name.c_str()), vec[0], vec[1], vec[2], vec[3]);
}
