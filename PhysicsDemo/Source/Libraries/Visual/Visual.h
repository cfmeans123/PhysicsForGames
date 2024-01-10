#pragma once

#include "OpenGL/OpenGL.h"

#include "MeshData.h"

#include "Platform/PlatformCore.h"
#include "Platform/Timer.h"

#include <vector>

namespace jm
{
	static_assert(std::is_same_v<GLchar, char>);
	static_assert(std::is_same_v<const GLchar*, cstring>);

	static_assert(std::is_same_v<GLshort, i16>);
	static_assert(std::is_same_v<GLint, i32>);
	static_assert(std::is_same_v<GLint, GLsizei>);
	static_assert(std::is_same_v<GLushort, u16>);
	static_assert(std::is_same_v<GLhalf, u16>);
	static_assert(std::is_same_v<GLuint, u32>);

	static_assert(std::is_same_v<GLfloat, f32>);
	static_assert(std::is_same_v<GLdouble, f64>);
	static_assert(std::is_same_v<GLclampf, f32>);
	static_assert(std::is_same_v<GLclampd, f64>);

	static_assert(std::is_same_v<GLvoid, void>);
}

namespace jm::Visual
{
	struct ShaderProgram
	{
		ShaderProgram(cstring vertexSource, std::vector<const GLchar*> varyingParameters);
		ShaderProgram(cstring vertexSource, cstring fragmentSource);
		~ShaderProgram();

		void MakeActive() const;

		void SetUniform(const GLchar* uniformName, GLfloat uniformValue) const;
		void SetUniform(const GLchar* uniformName, GLint uniformValue) const;
		void SetUniform(const GLchar* uniformName, GLuint uniformValue) const;
		void SetUniform(const GLchar* uniformName, math::vector2<GLint> const& uniformValue) const;
		void SetUniform(const GLchar* uniformName, math::vector3<GLfloat> const& uniformValue) const;
		void SetUniform(const GLchar* uniformName, math::matrix33<GLfloat> const& uniformValue) const;
		void SetUniform(const GLchar* uniformName, math::matrix44<GLfloat> const& uniformValue) const;

	private:

		const GLuint programHandle;
	};
}