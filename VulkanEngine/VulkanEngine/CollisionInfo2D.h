#pragma once
#include "pch.h"

class CollisionInfo2D 
{
private:
	float mDepth;
	glm::vec2 mNormal;
	glm::vec2 mEnd;


public:
	glm::vec2 mStart;

	CollisionInfo2D();

	void setNormal(glm::vec2 n);

	float getDepth();

	glm::vec2 getNormal();

	void setInfo(float depth, glm::vec2 normal, glm::vec2 start);

	void changeDir();
};