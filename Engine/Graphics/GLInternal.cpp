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
#undef glUniform1i
#undef glUniform1f
#undef glUniform4f
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
#undef glGenTextures
#undef glBindTexture
#undef glTexImage2D
#undef glTexParameteri
#undef glActiveTexture
#undef glDeleteTextures
#undef glTexSubImage2D
#undef glGenFramebuffers
#undef glBindFramebuffer
#undef glFramebufferTexture2D
#undef glCheckFramebufferStatus
#undef glDeleteFramebuffers
#undef glGenRenderbuffers
#undef glBindRenderbuffer
#undef glRenderbufferStorage
#undef glFramebufferRenderbuffer
#undef glDeleteRenderbuffers
#undef glDrawBuffers

namespace rex::gl::internal {
    #define X(type, name) type ptr_##name = nullptr;
    GL_POINTERS
    #undef X
}
