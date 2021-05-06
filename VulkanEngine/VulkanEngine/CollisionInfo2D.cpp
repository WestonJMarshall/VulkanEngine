#include "pch.h"
#include "CollisionInfo2D.h"

CollisionInfo2D::CollisionInfo2D() 
{
	mDepth = 0;
	mNormal = glm::vec3(0, 0, 0);
	mStart = glm::vec3(0, 0, 0);
	mEnd = glm::vec3(0, 0, 0);
}

void CollisionInfo2D::setNormal(glm::vec2 n) 
{
	mNormal = n;
}

float CollisionInfo2D::getDepth() 
{
	return mDepth;
}

glm::vec2 CollisionInfo2D::getNormal() 
{
	return mNormal;
}

void CollisionInfo2D::setInfo(float depth, glm::vec2 normal, glm::vec2 start)
{
	mDepth = depth;
	mNormal = normal;
	mStart = start;
	mEnd = start + (normal * depth);
}

void CollisionInfo2D::changeDir() 
{
	mNormal *= -1;
	glm::vec2 n = mStart;
	mStart = mEnd;
	mEnd = n;
}