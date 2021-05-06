#pragma once
#include "pch.h"
#include "RigidShape.h"
#include "CollisionInfo2D.h"
#include "2DVectorMath.h"
#include <string>

class Circle : public RigidShape 
{
private:
	std::string mType;
	float mRadius;
	glm::vec2 mStartPoint;
	VectorMath2D vecMath;

public:
	Circle(glm::vec2 center, float mass, float friction, float restitution, float radius) :RigidShape(center, mass, friction, restitution) 
	{
		mType = "Circle";
		mRadius = radius;
		mStartPoint = glm::vec2(center.x, center.y - radius);
		
	}

	void move(glm::vec2 moveVec) override;

	void rotate(float angle) override;

	void updateInertia();

	void update() override;

	std::string getType() override;

	glm::vec2 getCenter() override;

	//float getInvMass() override;

	glm::vec2 getVelocity() override;

	float getAngularVelocity() override;

	float getRestitution() override;

	float getFriction() override;

	float getInertia() override;

	float getRotation() override;

	void setAngularVelocity(float aV) override;

	void setVelocity(glm::vec2 nV) override;
};