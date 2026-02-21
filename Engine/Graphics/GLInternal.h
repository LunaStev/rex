#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>

// Fallback typedefs for platforms where SDL headers don't provide core PFNGL* types
#ifndef PFNGLGENTEXTURESPROC
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
#endif
#ifndef PFNGLBINDTEXTUREPROC
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
#endif
#ifndef PFNGLTEXIMAGE2DPROC
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(
    GLenum target, GLint level, GLint internalformat,
    GLsizei width, GLsizei height, GLint border,
    GLenum format, GLenum type, const void* pixels);
#endif
#ifndef PFNGLTEXPARAMETERIPROC
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
#endif
#ifndef PFNGLACTIVETEXTUREPROC
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
#endif
#ifndef PFNGLDELETETEXTURESPROC
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
#endif
#ifndef PFNGLGENFRAMEBUFFERSPROC
typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* ids);
#endif
#ifndef PFNGLBINDFRAMEBUFFERPROC
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
#endif
#ifndef PFNGLFRAMEBUFFERTEXTURE2DPROC
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DPROC)(
    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
#endif
#ifndef PFNGLCHECKFRAMEBUFFERSTATUSPROC
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
#endif
#ifndef PFNGLDELETEFRAMEBUFFERSPROC
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
#endif
#ifndef PFNGLGENRENDERBUFFERSPROC
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSPROC)(GLsizei n, GLuint* renderbuffers);
#endif
#ifndef PFNGLBINDRENDERBUFFERPROC
typedef void (APIENTRYP PFNGLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
#endif
#ifndef PFNGLRENDERBUFFERSTORAGEPROC
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEPROC)(
    GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
#endif
#ifndef PFNGLFRAMEBUFFERRENDERBUFFERPROC
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFERPROC)(
    GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
#endif
#ifndef PFNGLDELETERENDERBUFFERSPROC
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSPROC)(GLsizei n, const GLuint* renderbuffers);
#endif
#ifndef PFNGLDRAWBUFFERSPROC
typedef void (APIENTRYP PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum* bufs);
#endif
#ifndef PFNGLUNIFORM1IPROC
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
#endif
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
        X(PFNGLUNIFORM1IPROC, Uniform1i) \
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
        X(PFNGLDELETEBUFFERSPROC, DeleteBuffers) \
        X(PFNGLGENTEXTURESPROC, GenTextures) \
        X(PFNGLBINDTEXTUREPROC, BindTexture) \
        X(PFNGLTEXIMAGE2DPROC, TexImage2D) \
        X(PFNGLTEXPARAMETERIPROC, TexParameteri) \
        X(PFNGLACTIVETEXTUREPROC, ActiveTexture) \
        X(PFNGLDELETETEXTURESPROC, DeleteTextures) \
        X(PFNGLGENFRAMEBUFFERSPROC, GenFramebuffers) \
        X(PFNGLBINDFRAMEBUFFERPROC, BindFramebuffer) \
        X(PFNGLFRAMEBUFFERTEXTURE2DPROC, FramebufferTexture2D) \
        X(PFNGLCHECKFRAMEBUFFERSTATUSPROC, CheckFramebufferStatus) \
        X(PFNGLDELETEFRAMEBUFFERSPROC, DeleteFramebuffers) \
        X(PFNGLGENRENDERBUFFERSPROC, GenRenderbuffers) \
        X(PFNGLBINDRENDERBUFFERPROC, BindRenderbuffer) \
        X(PFNGLRENDERBUFFERSTORAGEPROC, RenderbufferStorage) \
        X(PFNGLFRAMEBUFFERRENDERBUFFERPROC, FramebufferRenderbuffer) \
        X(PFNGLDELETERENDERBUFFERSPROC, DeleteRenderbuffers) \
        X(PFNGLDRAWBUFFERSPROC, DrawBuffers)

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
#define glUniform1i ::rex::gl::internal::ptr_Uniform1i
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
#define glGenTextures ::rex::gl::internal::ptr_GenTextures
#define glBindTexture ::rex::gl::internal::ptr_BindTexture
#define glTexImage2D ::rex::gl::internal::ptr_TexImage2D
#define glTexParameteri ::rex::gl::internal::ptr_TexParameteri
#define glActiveTexture ::rex::gl::internal::ptr_ActiveTexture
#define glDeleteTextures ::rex::gl::internal::ptr_DeleteTextures
#define glGenFramebuffers ::rex::gl::internal::ptr_GenFramebuffers
#define glBindFramebuffer ::rex::gl::internal::ptr_BindFramebuffer
#define glFramebufferTexture2D ::rex::gl::internal::ptr_FramebufferTexture2D
#define glCheckFramebufferStatus ::rex::gl::internal::ptr_CheckFramebufferStatus
#define glDeleteFramebuffers ::rex::gl::internal::ptr_DeleteFramebuffers
#define glGenRenderbuffers ::rex::gl::internal::ptr_GenRenderbuffers
#define glBindRenderbuffer ::rex::gl::internal::ptr_BindRenderbuffer
#define glRenderbufferStorage ::rex::gl::internal::ptr_RenderbufferStorage
#define glFramebufferRenderbuffer ::rex::gl::internal::ptr_FramebufferRenderbuffer
#define glDeleteRenderbuffers ::rex::gl::internal::ptr_DeleteRenderbuffers
#define glDrawBuffers ::rex::gl::internal::ptr_DrawBuffers

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
