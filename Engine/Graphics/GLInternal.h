#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <functional>
#include <iostream>

// Use a private namespace for the actual function pointers
namespace rex::gl::internal {
    // Define the pointers with a unique prefix to avoid conflicts with system headers
    #define GL_POINTERS \
        X(PFNGLCREATEPROGRAMPROC, CreateProgram) \
        X(PFNGLCREATESHADERPROC, CreateShader) \
        X(PFNGLSHADERSOURCEPROC, ShaderSource) \
        X(PFNGLCOMPILESHADERPROC, CompileShader) \
        X(PFNGLATTACHSHADERPROC, AttachShader) \
        X(PFNGLLINKPROGRAMPROC, LinkProgram) \
        X(PFNGLUSEPROGRAMPROC, UseProgram) \
        X(PFNGLGETUNIFORMLOCATIONPROC, GetUniformLocation) \
        X(PFNGLUNIFORM1FPROC, Uniform1f) \
        X(PFNGLUNIFORM3FPROC, Uniform3f) \
        X(PFNGLUNIFORM3FVPROC, Uniform3fv) \
        X(PFNGLUNIFORMMATRIX4FVPROC, UniformMatrix4fv) \
        X(PFNGLDELETESHADERPROC, DeleteShader) \
        X(PFNGLDELETEPROGRAMPROC, DeleteProgram) \
        X(PFNGLGENVERTEXARRAYSPROC, GenVertexArrays) \
        X(PFNGLGENBUFFERSPROC, GenBuffers) \
        X(PFNGLBINDVERTEXARRAYPROC, BindVertexArray) \
        X(PFNGLBINDBUFFERPROC, BindBuffer) \
        X(PFNGLBUFFERDATAPROC, BufferData) \
        X(PFNGLVERTEXATTRIBPOINTERPROC, VertexAttribPointer) \
        X(PFNGLENABLEVERTEXATTRIBARRAYPROC, EnableVertexAttribArray) \
        X(PFNGLGETSHADERIVPROC, GetShaderiv) \
        X(PFNGLGETSHADERINFOLOGPROC, GetShaderInfoLog) \
        X(PFNGLGETPROGRAMIVPROC, GetProgramiv) \
        X(PFNGLGETPROGRAMINFOLOGPROC, GetProgramInfoLog) \
        X(PFNGLDELETEVERTEXARRAYSPROC, DeleteVertexArrays) \
        X(PFNGLDELETEBUFFERSPROC, DeleteBuffers)

    #define X(type, name) extern type ptr_##name;
    GL_POINTERS
    #undef X
}

// Redirect standard names to our pointers
// We only do this for the "modern" functions that aren't usually in GL 1.1
#define glCreateProgram ::rex::gl::internal::ptr_CreateProgram
#define glCreateShader ::rex::gl::internal::ptr_CreateShader
#define glShaderSource ::rex::gl::internal::ptr_ShaderSource
#define glCompileShader ::rex::gl::internal::ptr_CompileShader
#define glAttachShader ::rex::gl::internal::ptr_AttachShader
#define glLinkProgram ::rex::gl::internal::ptr_LinkProgram
#define glUseProgram ::rex::gl::internal::ptr_UseProgram
#define glGetUniformLocation ::rex::gl::internal::ptr_GetUniformLocation
#define glUniform1f ::rex::gl::internal::ptr_Uniform1f
#define glUniform3f ::rex::gl::internal::ptr_Uniform3f
#define glUniform3fv ::rex::gl::internal::ptr_Uniform3fv
#define glUniformMatrix4fv ::rex::gl::internal::ptr_UniformMatrix4fv
#define glDeleteShader ::rex::gl::internal::ptr_DeleteShader
#define glDeleteProgram ::rex::gl::internal::ptr_DeleteProgram
#define glGenVertexArrays ::rex::gl::internal::ptr_GenVertexArrays
#define glGenBuffers ::rex::gl::internal::ptr_GenBuffers
#define glBindVertexArray ::rex::gl::internal::ptr_BindVertexArray
#define glBindBuffer ::rex::gl::internal::ptr_BindBuffer
#define glBufferData ::rex::gl::internal::ptr_BufferData
#define glVertexAttribPointer ::rex::gl::internal::ptr_VertexAttribPointer
#define glEnableVertexAttribArray ::rex::gl::internal::ptr_EnableVertexAttribArray
#define glGetShaderiv ::rex::gl::internal::ptr_GetShaderiv
#define glGetShaderInfoLog ::rex::gl::internal::ptr_GetShaderInfoLog
#define glGetProgramiv ::rex::gl::internal::ptr_GetProgramiv
#define glGetProgramInfoLog ::rex::gl::internal::ptr_GetProgramInfoLog
#define glDeleteVertexArrays ::rex::gl::internal::ptr_DeleteVertexArrays
#define glDeleteBuffers ::rex::gl::internal::ptr_DeleteBuffers

typedef void* (*RexGLLoaderFunc)(const char* name);

inline void loadGLFunctions(RexGLLoaderFunc loader) {
    #define X(type, name) ::rex::gl::internal::ptr_##name = (type)loader("gl" #name); \
        if (!::rex::gl::internal::ptr_##name) std::cerr << "[GL] Critical: Failed to load gl" << #name << "\n";
    GL_POINTERS
    #undef X
}

inline void loadGLFunctionsSDL() {
    loadGLFunctions((RexGLLoaderFunc)SDL_GL_GetProcAddress);
}

// Ensure basic GL types and constants
#ifndef GL_COLOR_BUFFER_BIT
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_TRIANGLES 0x0004
#define GL_FLOAT 0x1406
#define GL_UNSIGNED_INT 0x1405
#define GL_FALSE 0
#define GL_TRUE 1
#endif
