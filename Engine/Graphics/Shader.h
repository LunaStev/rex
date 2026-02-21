#pragma once
#include "GLInternal.h"
#include "../Core/RexMath.h"
#include <string>

namespace rex {

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();

    void bind() const;
    void unbind() const;

    void setUniform(const std::string& name, int value);
    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, const Vec3& value);
    void setUniform(const std::string& name, const Mat4& value);

private:
    uint32_t m_id;
    uint32_t compileShader(uint32_t type, const std::string& src);
};

}
