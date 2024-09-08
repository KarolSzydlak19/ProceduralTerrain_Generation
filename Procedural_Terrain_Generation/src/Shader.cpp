#include"Shader.h"
#include <vector>

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

bool Shader::fileExists(const std::string& filename) {
	std::ifstream file(filename);
	return file.good();
}

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	if (!fileExists(vertexFile)) {
		std::cerr << "Vertex shader file not found: " << vertexFile << std::endl;
		return;
	}

	if (!fileExists(fragmentFile)) {
		std::cerr << "Fragment shader file not found: " << fragmentFile << std::endl;
		return;
	}

	// Read vertexFile and fragmentFile and store the strings
	std::cout << "Loading vertex shader: " << vertexFile << std::endl;
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);
	std::cout << "Vertex shader loaded successfully" << std::endl;

	// Convert the shader source strings into character arrays
	std::cout << "Loading fragment shader: " << fragmentFile << std::endl;
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();
	std::cout << "Fragment shader loaded successfully" << std::endl;

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);
	// Checks if Shader compiled succesfully
	compileErrors(vertexShader, "VERTEX");

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);
	// Checks if Shader compiled succesfully
	compileErrors(fragmentShader, "FRAGMENT");

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	// Checks if Shaders linked succesfully
	compileErrors(ID, "PROGRAM");

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	std::cout << vertexFile << " " << fragmentFile << " " << ID << std::endl;

}

// Activates the Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
	GLenum error = glGetError();
	if (glGetError() != GL_NO_ERROR) {
		std::cerr << "Error activating shader ID " << ID << ": " << error << std::endl;
	}
}

// Deletes the Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// Checks if the different Shaders have compiled properly
void Shader::compileErrors(unsigned int shader, const char* type)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}


void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
	}
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	GLint location = glGetUniformLocation(ID, name.c_str());
	if (location == -1) {
		std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;
	}
	glUniform3fv(location, 1, &value[0]);
}
