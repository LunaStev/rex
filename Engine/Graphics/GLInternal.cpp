#include "GLInternal.h"

// Undefine the redirection macros so we can define the actual pointers
#undef glCreateProgram
#undef glCreateShader
#undef glShaderSource
#undef glCompileShader
#undef glAttachShader
#undef glLinkProgram
#undef glUseProgram
#undef glGetUniformLocation
#undef glUniform1f
#undef glUniform3f
#undef glUniform3fv
#undef glUniformMatrix4fv
#undef glDeleteShader
#undef glDeleteProgram
#undef glGenVertexArrays
#undef glGenBuffers
#undef glBindVertexArray
#undef glBindBuffer
#undef glBufferData
#undef glVertexAttribPointer
#undef glEnableVertexAttribArray
#undef glGetShaderiv
#undef glGetShaderInfoLog
#undef glGetProgramiv
#undef glGetProgramInfoLog
#undef glDeleteVertexArrays
#undef glDeleteBuffers

namespace rex::gl::internal {
    #define X(type, name) type ptr_##name = nullptr;
    GL_POINTERS
    #undef X
}
