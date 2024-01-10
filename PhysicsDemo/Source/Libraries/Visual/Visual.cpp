#include "Visual.h"

#include "VisualDebug.h"

#include "RenderingContext.h"

namespace jm::Visual
{
	GLuint CompileShader(GLenum shaderType, const GLchar* sourceString)
	{
		GLuint shaderHandle = glCreateShader(shaderType);
		glShaderSource(shaderHandle, 1, &sourceString, NULL);

		glCompileShader(shaderHandle);

		// check for shader compile errors
		int success;
		char infoLog[512];
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderHandle, 512, NULL, infoLog);
			JM_VISUAL_HALT("Shader compile failed! : %s", infoLog);
		}
		OpenGL::CheckError();

		return shaderHandle;
	}

	void LinkProgram(GLuint programHandle, GLuint vertexShader)
	{
		glAttachShader(programHandle, vertexShader);
		glLinkProgram(programHandle);

		// check for linking errors
		int success;
		char infoLog[512];
		glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programHandle, 512, NULL, infoLog);
			JM_VISUAL_HALT("Shader program link failed! : %s", infoLog);
		}
		OpenGL::CheckError();
	}

	void LinkProgram(GLuint programHandle, GLuint vertexShader, GLuint fragmentShader)
	{
		glAttachShader(programHandle, vertexShader);
		glAttachShader(programHandle, fragmentShader);
		glLinkProgram(programHandle);

		// check for linking errors
		int success;
		char infoLog[512];
		glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programHandle, 512, NULL, infoLog);
			JM_VISUAL_HALT("Shader program link failed! : %s", infoLog);
		}
		OpenGL::CheckError();
	}

	ShaderProgram::ShaderProgram(cstring vertexSource, std::vector<const GLchar*> varyingParameters)
		: programHandle(glCreateProgram())
	{
		int vertexShader = Visual::CompileShader(GL_VERTEX_SHADER, vertexSource);

		glTransformFeedbackVaryings(programHandle, (GLsizei)varyingParameters.size(), varyingParameters.data(), GL_INTERLEAVED_ATTRIBS);

		LinkProgram(programHandle, vertexShader);
		glDeleteShader(vertexShader);
		OpenGL::CheckError();

		MakeActive();
	}

	ShaderProgram::ShaderProgram(cstring vertexSource, cstring fragmentSource)
		: programHandle(glCreateProgram())
	{
		int vertexShader = Visual::CompileShader(GL_VERTEX_SHADER, vertexSource);

		int fragmentShader = Visual::CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

		LinkProgram(programHandle, vertexShader, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		OpenGL::CheckError();

		MakeActive();
	}

	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(programHandle);
	}

	void ShaderProgram::MakeActive() const
	{
		glUseProgram(programHandle);
		OpenGL::CheckError();
	}

	GLint LocateUniform(GLuint program, const GLchar* uniformName)
	{
		GLint location = glGetUniformLocation(program, uniformName);
		JM_VISUAL_ASSERT(location != -1, "No uniform location %s found!", uniformName);
		OpenGL::CheckError();
		return location;
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, GLfloat uniformValue) const
	{
		glUniform1f(LocateUniform(programHandle, uniformName), uniformValue);
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, GLint uniformValue) const
	{
		glUniform1i(LocateUniform(programHandle, uniformName), uniformValue);
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, GLuint uniformValue) const
	{
		glUniform1ui(LocateUniform(programHandle, uniformName), uniformValue);
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, math::vector2<GLint> const& uniformValue) const
	{
		glUniform2i(LocateUniform(programHandle, uniformName), uniformValue.x, uniformValue.y);
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, math::vector3<GLfloat> const& uniformValue) const
	{
		glUniform3f(LocateUniform(programHandle, uniformName), uniformValue.x, uniformValue.y, uniformValue.z);
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, math::matrix33<GLfloat> const& uniformValue) const
	{
		glUniformMatrix3fv(LocateUniform(programHandle, uniformName), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&uniformValue));
	}

	void ShaderProgram::SetUniform(const GLchar* uniformName, math::matrix44<GLfloat> const& uniformValue) const
	{
		glUniformMatrix4fv(LocateUniform(programHandle, uniformName), 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&uniformValue));
	}
}