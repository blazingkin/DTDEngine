
#pragma  once

#ifndef LAB471_PROGRAM_H_INCLUDED
#define LAB471_PROGRAM_H_INCLUDED

#include <map>
#include <string>
#include <memory>
#include <vector>
#include "Texture.h"
#include "Material.h"

#include <glad/glad.h>


std::string readFileAsString(const std::string &fileName);
void compile_vertex_shader(std::string resourceDirectory, std::string name);
void compile_fragment_shader(std::string resourceDirectory, std::string name);

class Program
{

public:

	void setVerbose(const bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }

	virtual bool init(std::vector<std::string> shaders);
	virtual void bind();
	virtual void unbind();

	bool supportsInstanceRendering = false;

	void addAttribute(const std::string &name);
	void addUniform(const std::string &name);
	bool hasAttribute(const std::string &name);
	GLint getAttribute(const std::string &name) const;
	GLint getUniform(const std::string &name) const;
	bool hasUniform(const std::string &name);
	void enableInstanceRendering();
	void setInstanceValues(std::string attr, std::shared_ptr<std::vector<glm::vec4>> data);
	void setInstanceModels(std::vector<glm::mat4> *data);
	void setMaterial(std::shared_ptr<Material> m);
	void setTexture(std::shared_ptr<Texture> tex);
	void setBumpMap(std::shared_ptr<Texture> tex);
	void textureEnabled(bool b);
	void setNormalMap(std::shared_ptr<Texture> tex);
	~Program();

private:

	GLuint pid = 0;
	std::map<std::string, GLint> attributes;
	std::map<std::string, GLint> uniforms;
	std::map<std::string, GLuint> atBufIds;
	bool verbose = true;

};

#endif // LAB471_PROGRAM_H_INCLUDED
