#pragma once

#include "Platform/OS.h"

#include <cstddef>

#define GLDECL WINAPI

#include <GL/gl.h>
#include "KHR/glext.h"
#include "KHR/wglext.h"

//reference: http://docs.gl/
#define GL_LIST_WIN32 \
 \
    GLE(void,			BlendEquation,				GLenum mode) \
    GLE(void,			ActiveTexture,				GLenum texture)

#define GL_LIST \
 \
	GLE(void,			AttachShader,				GLuint program, GLuint shader) \
	GLE(void,			BeginTransformFeedback,		GLenum primitiveMode) \
	GLE(void,			BindBuffer,					GLenum target, GLuint buffer) \
	GLE(void,			BindBufferBase,				GLenum target, GLuint index, GLuint buffer) \
	GLE(void,			BindFramebuffer,			GLenum target, GLuint framebuffer) \
	GLE(void,			BindRenderbuffer,			GLenum target, GLuint renderbuffer) \
	GLE(void,			BindSampler,				GLuint unit, GLuint sampler) \
	GLE(void,			BindTransformFeedback,		GLuint target, GLuint feedbackbuffer) \
	GLE(void,			BindVertexArray,			GLuint array) \
	GLE(void,			BindVertexBuffer,			GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) \
	GLE(void,			BlendEquationSeparate,		GLenum modeRGB, GLenum modeAlpha) \
	GLE(void,			BlendFuncSeparate,			GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) \
	GLE(void,			BufferData,					GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) \
	GLE(void,			BufferSubData,				GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data) \
	GLE(GLenum,			CheckFramebufferStatus,		GLenum target) \
	GLE(void,			ClearBufferfv,				GLenum buffer, GLint drawbuffer, const GLfloat * value) \
	GLE(void,			CompileShader,				GLuint shader) \
	GLE(GLuint,			CreateProgram,				void) \
	GLE(GLuint,			CreateShader,				GLenum type) \
	GLE(void,			DeleteBuffers,				GLsizei n, const GLuint *buffers) \
	GLE(void,			DeleteFramebuffers,			GLsizei n, const GLuint *framebuffers) \
	GLE(void,			DeleteRenderbuffers,		GLsizei n, const GLuint *renderbuffers) \
	GLE(void,			DeleteProgram,				GLuint program) \
	GLE(void,			DeleteShader,				GLuint shader) \
	GLE(void,			DeleteVertexArrays,			GLsizei n, const GLuint *arrays) \
	GLE(void,			DetachShader,				GLuint program, GLuint shader) \
	GLE(void,			DisableVertexAttribArray,	GLuint index) \
	GLE(void,			DrawBuffers,				GLsizei n, const GLenum *bufs) \
	GLE(void,			DrawElementsBaseVertex,		GLenum mode, GLsizei count, GLenum type, GLvoid *indices, GLint basevertex) \
	GLE(void,			DrawTransformFeedback,		GLenum mode, GLuint feedbackbuffer) \
	GLE(void,			EnableVertexAttribArray,	GLuint index) \
	GLE(void,			EndTransformFeedback,		void) \
	GLE(void,			FramebufferRenderbuffer,	GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) \
	GLE(void,			FramebufferTexture2D,		GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) \
	GLE(void,			GenBuffers,					GLsizei n, GLuint *buffers) \
	GLE(void,			GenFramebuffers,			GLsizei n, GLuint * framebuffers) \
	GLE(void,			GenRenderbuffers,			GLsizei n, GLuint * renderbuffers) \
	GLE(void,			GenTransformFeedbacks,		GLsizei n, GLuint * feedbackbuffers) \
	GLE(void,			GenVertexArrays,			GLsizei n, GLuint *arrays) \
	GLE(void,			GenerateMipmap,				GLenum target) \
	GLE(void,			GenerateTextureMipmap,		GLuint texture) \
	GLE(GLint,			GetAttribLocation,			GLuint program, const GLchar *name) \
	GLE(void,			GetProgramInfoLog,			GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) \
	GLE(void,			GetProgramiv,				GLuint program, GLenum pname, GLint *params) \
	GLE(void,			GetShaderInfoLog,			GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) \
	GLE(void,			GetShaderiv,				GLuint shader, GLenum pname, GLint *params) \
	GLE(const GLubyte*,	GetStringi,					GLenum name, GLuint index) \
	GLE(GLint,			GetUniformLocation,			GLuint program, const GLchar *name) \
	GLE(void,			LinkProgram,				GLuint program) \
	GLE(void,			RenderbufferStorage,		GLenum target, GLenum internalformat, GLsizei width, GLsizei height) \
	GLE(void,			ShaderSource,				GLuint shader, GLsizei count, const GLchar **string, const GLint *length) \
	GLE(void,			TransformFeedbackVaryings,	GLuint program, GLsizei count, const GLchar **varyings, GLenum bufferMode) \
	GLE(void,			Uniform1i,					GLint location, GLint v0) \
	GLE(void,			Uniform2i,					GLint location, GLint v0, GLint v1) \
	GLE(void,			Uniform1ui,					GLint location, GLuint v0) \
	GLE(void,			Uniform1f,					GLint location, GLfloat v0) \
	GLE(void,			Uniform2f,					GLint location, GLfloat v0, GLfloat v1) \
	GLE(void,			Uniform3f,					GLint location, GLfloat v0, GLfloat v1, GLfloat v2) \
	GLE(void,			Uniform4f,					GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) \
	GLE(void,			UniformMatrix3fv,			GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
	GLE(void,			UniformMatrix4fv,			GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) \
	GLE(void,			UseProgram,					GLuint program) \
	GLE(void,			VertexAttribBinding,		GLuint attribindex, GLuint bindingindex) \
	GLE(void,			VertexAttribFormat,			GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) \
	GLE(void,			VertexAttribPointer,		GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)

#define GLE(ret, name, ...) \
typedef ret GLDECL name##Procedure(__VA_ARGS__); \
extern name##Procedure * gl##name;

GL_LIST
GL_LIST_WIN32
#undef GLE

extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

#include <GL/glu.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
