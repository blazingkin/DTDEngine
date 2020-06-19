
#include "Program.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "GLSL.h"

static std::map<std::string, GLuint> shaders;

void compile_shader(std::string resourceDirectory, std::string name, uint64_t shader_type) {
	std::string path = resourceDirectory + "/" + name + "";
	std::string shaderString = readFileAsString(path);
	const char *shader = shaderString.c_str();
	GLuint S = glCreateShader(shader_type);
	CHECKED_GL_CALL(glShaderSource(S, 1, &shader, NULL));
	CHECKED_GL_CALL(glCompileShader(S));
	GLint rc;
	CHECKED_GL_CALL(glGetShaderiv(S, GL_COMPILE_STATUS, &rc));
	if (rc == GL_FALSE)
	{
		GLSL::printShaderInfoLog(S);
		std::cout << "Error compiling vertex shader " << name << std::endl;
		std::exit(1);
	}
	shaders.insert(std::pair<std::string, GLuint>(name, S));
}

void compile_vertex_shader(std::string resourceDirectory, std::string name) {
	compile_shader(resourceDirectory, name, GL_VERTEX_SHADER);
}

void compile_fragment_shader(std::string resourceDirectory, std::string name) {
	compile_shader(resourceDirectory, name, GL_FRAGMENT_SHADER);
}


std::string readFileAsString(const std::string &fileName)
{
	std::string result;
	std::ifstream fileHandle(fileName);

	if (fileHandle.is_open())
	{
		fileHandle.seekg(0, std::ios::end);
		result.reserve((size_t) fileHandle.tellg());
		fileHandle.seekg(0, std::ios::beg);

		result.assign((std::istreambuf_iterator<char>(fileHandle)), std::istreambuf_iterator<char>());
	}
	else
	{
		std::cerr << "Could not open file: '" << fileName << "'" << std::endl;
	}

	return result;
}

void Program::setMaterial(std::shared_ptr<Material> m) {
	if (hasUniform("MatAmb")) {
		glUniform3f(getUniform("MatAmb"), m->Ambient.r, m->Ambient.g, m->Ambient.b);
		glUniform3f(getUniform("MatDif"), m->Diffuse.r, m->Diffuse.g, m->Diffuse.b);
		glUniform3f(getUniform("MatSpec"), m->Specular.r, m->Specular.g, m->Specular.b);
		glUniform1f(getUniform("shine"), m->shine);
	}
}

void Program::setTexture(std::shared_ptr<Texture> tex) {
	tex->bind(getUniform("Texture0"));
}

void Program::setNormalMap(std::shared_ptr<Texture> tex) {
	if (hasUniform("normalMap")) {
		tex->bind(getUniform("normalMap"));
	}
}
void Program::setBumpMap(std::shared_ptr<Texture> tex) {
	if (hasUniform("bumpMap")) {
		tex->bind(getUniform("bumpMap"));
	}
}

void Program::textureEnabled(bool enabled) {
	if (hasUniform("textureEnabled")) {
		glUniform1i(getUniform("textureEnabled"), enabled ? 1 : 0);
	}
} 

bool Program::init(std::vector<std::string> pShaders)
{
	GLint rc;

	// Create the program and link
	pid = glCreateProgram();
	for (std::string shaderName : pShaders) {
		if (shaders.count(shaderName) == 0) {
			std::cerr << "Shader named " << shaderName << " was never compiled!" << std::endl;
			std::exit(1);
		}
		CHECKED_GL_CALL(glAttachShader(pid, shaders[shaderName]));
	}
	CHECKED_GL_CALL(glLinkProgram(pid));
	CHECKED_GL_CALL(glGetProgramiv(pid, GL_LINK_STATUS, &rc));
	if (!rc)
	{
		if (isVerbose())
		{
			GLSL::printProgramInfoLog(pid);
			std::cout << "Error linking shaders " << std::endl;
			std::exit(1);
		}
		return false;
	}

	return true;
}
static int bound = 0;

void Program::bind()
{
	if (bound != 0) {
		std::exit(1);
	}
	CHECKED_GL_CALL(glUseProgram(pid));
}

void Program::enableInstanceRendering() {
	supportsInstanceRendering = true;

}


void Program::setInstanceModels(std::vector<glm::mat4> *data) {
	auto h_M = getAttribute("M");
	auto bufId = atBufIds["M"];
	// Bind the buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, bufId));
	// Send the model matrices to the gpu
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, data->size() * sizeof(glm::mat4), glm::value_ptr(data->at(0)), GL_STATIC_DRAW));
	
	CHECKED_GL_CALL(glEnableVertexAttribArray(h_M));	
	CHECKED_GL_CALL(glVertexAttribPointer(h_M, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0));

	CHECKED_GL_CALL(glEnableVertexAttribArray(h_M+1));	
	CHECKED_GL_CALL(glVertexAttribPointer(h_M+1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4))));

	CHECKED_GL_CALL(glEnableVertexAttribArray(h_M+2));	
	CHECKED_GL_CALL(glVertexAttribPointer(h_M+2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4))));

	CHECKED_GL_CALL(glEnableVertexAttribArray(h_M+3));	
	CHECKED_GL_CALL(glVertexAttribPointer(h_M+3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4))));


	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// Set this attribute to change once per instance
	glVertexAttribDivisor(h_M, 1);
	glVertexAttribDivisor(h_M + 1, 1);
	glVertexAttribDivisor(h_M + 2, 1);
	glVertexAttribDivisor(h_M + 3, 1);
}


void Program::setInstanceValues(std::string attr, std::shared_ptr<std::vector<glm::vec4>> data) {
	auto h_M = getAttribute(attr);
	auto bufId = atBufIds[attr];
	// Bind the buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, bufId));
	// Send the model matrices to the gpu
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, data->size() * sizeof(glm::vec4), glm::value_ptr(data->at(0)), GL_STATIC_DRAW));
	
	CHECKED_GL_CALL(glEnableVertexAttribArray(h_M));	

	CHECKED_GL_CALL(glVertexAttribPointer(h_M, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void *)0));


	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));

	// Set this attribute to change once per instance
	glVertexAttribDivisor(h_M, 1);
}

void Program::unbind()
{
	bound = 0;
	CHECKED_GL_CALL(glUseProgram(0));
}

void Program::addAttribute(const std::string &name)
{
	attributes[name] = GLSL::getAttribLocation(pid, name.c_str(), isVerbose());
	GLuint bufId;
	CHECKED_GL_CALL(glGenBuffers(1, &bufId));
	atBufIds[name] = bufId;
}

bool Program::hasAttribute(const std::string &name) {
	return attributes.find(name.c_str()) != attributes.end();
}

bool Program::hasUniform(const std::string &name) {
	return uniforms.find(name.c_str()) != uniforms.end();
}

void Program::addUniform(const std::string &name)
{
	uniforms[name] = GLSL::getUniformLocation(pid, name.c_str(), isVerbose());
}

GLint Program::getAttribute(const std::string &name) const
{
	std::map<std::string, GLint>::const_iterator attribute = attributes.find(name.c_str());
	if (attribute == attributes.end())
	{
		if (isVerbose())
		{
			std::cout << name << " is not an attribute variable" << std::endl;
		}
		return -1;
	}
	return attribute->second;
}

GLint Program::getUniform(const std::string &name) const
{
	std::map<std::string, GLint>::const_iterator uniform = uniforms.find(name.c_str());
	if (uniform == uniforms.end())
	{
		if (isVerbose())
		{
			std::cout << name << " is not a uniform variable" << std::endl;
		}
		return -1;
	}
	return uniform->second;
}

Program::~Program() 
{ 
  
} 
