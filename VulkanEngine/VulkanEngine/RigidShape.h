#pragma once
#include "pch.h"
#include "CollisionInfo2D.h"
#include <string>

class RigidShape 
{
protected:
	glm::vec2 mCenter;
	float mInertia;
	float mInvMass;
	float mRestitution;
	float mFriction;
	float mAngle = 0;
	float mAngularVelocity = 0;
	float mAngularAcceleration = 0;
	float mBoundRadius = 0;
	glm::vec2 mVelocity;
	glm::vec2 mAcceleration;
public:
	//contructor
	RigidShape(glm::vec2 center, float mass, float friction, float restitution);
	
	//update
	virtual void update();

	virtual void move(glm::vec2 moveVec);

	virtual void rotate(float angle);

	virtual glm::vec2 getCenter();

	virtual std::string getType();

	virtual float getInvMass();

	virtual glm::vec2 getVelocity();

	virtual float getAngularVelocity();

	virtual float getRestitution();

	virtual float getFriction();

	virtual float getInertia();

	virtual std::vector<glm::vec2> getFaceNormals();

	virtual std::vector<glm::vec2> getVertexes();

	virtual float getRotation();

	virtual void setAngularVelocity(float aV);

	virtual void setVelocity(glm::vec2 nV);

	virtual glm::vec2 normalize(glm::vec2 n);
};