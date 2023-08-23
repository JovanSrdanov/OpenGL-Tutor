#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {

public:
    static const unsigned POSITION_LOCATION = 0;
    static const unsigned COLOR_LOCATION = 1;
    unsigned mId;
    Shader(const std::string& vShaderPath, const std::string& fShaderPath);
    unsigned GetId() const;
    void SetUniform1i(const std::string& uniform, int v) const;
    void SetUniform1f(const std::string& uniform, float v) const;
    void SetUniform3f(const std::string& uniform, const glm::vec3& v) const;
    void SetUniform4m(const std::string& uniform, const glm::mat4& m) const;
    void SetModel(const glm::mat4& m) const;
    void SetView(const glm::mat4& m) const;
    void SetProjection(const glm::mat4& m) const;

private:
    unsigned loadAndCompileShader(std::string filename, GLuint shaderType);
    unsigned createBasicProgram(unsigned vShader, unsigned fShader);
};