#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ComputeShader {
public:
    unsigned int ID;

    ComputeShader(const char* computePath) {
        std::string computeCode;
        std::ifstream cShaderFile;
        cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
            cShaderFile.open(computePath);
            std::stringstream cShaderStream;
            cShaderStream << cShaderFile.rdbuf();
            cShaderFile.close();
            computeCode = cShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::COMPUTE_SHADER::FILE_NOT_SUCCESSFULLY_READ: " << computePath << std::endl;
            exit(-1);
        }
        const char* cShaderCode = computeCode.c_str();

        unsigned int compute = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(compute, 1, &cShaderCode, NULL);
        glCompileShader(compute);
        checkCompileErrors(compute, "COMPUTE");

        ID = glCreateProgram();
        glAttachShader(ID, compute);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        glDeleteShader(compute);
    }

    void use() const {
        glUseProgram(ID);
    }

    void dispatch(unsigned int numGroupsX) const {
        glDispatchCompute(numGroupsX, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::COMPUTE_SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::COMPUTE_PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
