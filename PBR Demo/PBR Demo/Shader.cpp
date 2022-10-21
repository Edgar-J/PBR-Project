#include "Shader.h"


//CONSTRUCTOR
//============
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	// Retrive shader source code
	//===========================
	std::string vertexCode;		
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);			// open files
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();	// read files buffer into streams
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();					// close file handlers
		fShaderFile.close();

		vertexCode = vShaderStream.str();		// convert stream into string
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	// Compile Shaders
	//================
	unsigned int vertex, fragment;	// create a variables to hold the shader IDs
	int success;					// used for error checking
	char infoLog[512];				// used hold error info logs

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);				// create vertex shader
	glShaderSource(vertex, 1, &vShaderCode, NULL);			// attach shader source code to shader object
	glCompileShader(vertex);								// compile shader
	
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);		// check if the shader has compiled succesfully and print a message if not
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);			// create fragment shader
	glShaderSource(fragment, 1, &fShaderCode, NULL);		// attach shader source code to shader object
	glCompileShader(fragment);								// compile shader

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);	// check for compilation errors
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// shader program
	ID = glCreateProgram();							// create a variable to hold the shader program's ID and create a shader program
	glAttachShader(ID, vertex);						// attach both shaders to the program
	glAttachShader(ID, fragment);

	glLinkProgram(ID);								// link attached shaders to the program
	glGetProgramiv(ID, GL_LINK_STATUS, &success);	// check for linking errors
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);		// delete shader objects
	glDeleteShader(fragment);
}



// FUNCTIONS
//==========
// activate/deactivate program
void Shader::use()
{
	glUseProgram(ID);
}
void Shader::stopUsing()
{
	glDeleteProgram(ID);
}

// find uniform location and set its value
void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}