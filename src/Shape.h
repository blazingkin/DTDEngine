#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_obj_loader/tiny_obj_loader.h>
#include "_instance_data.h"

class Program;

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void createShape(tinyobj::shape_t & shape);
	void init();
	void measure();
	void draw(const std::shared_ptr<Program> prog) const;
	void drawInstances(const std::shared_ptr<Program> prog, int instances, instance_render_data_t *data) const;
	void shift(glm::vec3 offset);
	void setNormalMapData();
	void scale(glm::vec3 scale);
	void ComputeTanBN(); 
	glm::vec3 min;
	glm::vec3 max;
	
private:
	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> tanBuf;
	std::vector<float> bnBuf;
	std::vector<float> texBuf;
	std::vector<float> bumpBuf;
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;
	unsigned bnBufID;
	unsigned tanBufID;
	unsigned bumpBufID;
   unsigned vaoID;
   bool initialized;
};


#endif
