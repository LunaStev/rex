#include "Shader.h"
#include "../Core/Logger.h"
#include <vector>

namespace rex {

Shader::Shader(const std::string& v, const std::string& f) {
    m_id = glCreateProgram();
    uint32_t vs = compileShader(GL_VERTEX_SHADER, v);
    uint32_t fs = compileShader(GL_FRAGMENT_SHADER, f);
    
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);
    
    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(m_id, 512, nullptr, info);
        Logger::error("Shader Linking Failed: {}", info);
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader() { glDeleteProgram(m_id); }

void Shader::bind() const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

void Shader::setUniform(const std::string& n, int v) { glUniform1i(glGetUniformLocation(m_id, n.c_str()), v); }
void Shader::setUniform(const std::string& n, float v) { glUniform1f(glGetUniformLocation(m_id, n.c_str()), v); }
void Shader::setUniform(const std::string& n, const Vec3& v) { glUniform3f(glGetUniformLocation(m_id, n.c_str()), v.x, v.y, v.z); }
void Shader::setUniform(const std::string& n, const Vec4& v) { glUniform4f(glGetUniformLocation(m_id, n.c_str()), v.x, v.y, v.z, v.w); }
void Shader::setUniform(const std::string& n, const Mat4& v) { glUniformMatrix4fv(glGetUniformLocation(m_id, n.c_str()), 1, GL_FALSE, v.m); }

uint32_t Shader::compileShader(uint32_t type, const std::string& src) {
    uint32_t id = glCreateShader(type);
    const char* c_src = src.c_str();
    glShaderSource(id, 1, &c_src, nullptr);
    glCompileShader(id);
    
    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(id, 512, nullptr, info);
        Logger::error("Shader Compilation Failed ({}): {}", (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), info);
    }
    return id;
}

}
