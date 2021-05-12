#include "pch.h"
#include "RigidShape.h"
#include "PhysicsManager.h"
#include "VulkanManager.h"

RigidShape::RigidShape(glm::vec2 center, float mass, float friction, float restitution) 
{
	mCenter = center;
	mInertia = 0;
	mInvMass = mass;
	
	if (friction == 0) {
		mFriction = 0.8;
	}
	else {
		mFriction = friction;
	}
	if (restitution == 0) {
		mRestitution = 0.2;
	}
	else {
		mRestitution = restitution;
	}
	mVelocity = glm::vec2(0, 0);
	if (mInvMass != 0) {
		mInvMass = 1 / mInvMass;
		mAcceleration = glm::vec2(0, -0.6);
	}
	else {
		mAcceleration = glm::vec2(0, 0);
	}

	mAngle = 0;
	mAngularVelocity = 0;
	mAngularAcceleration = 0;
	mBoundRadius = 0;
}

void RigidShape::update() {
	std::cout << "I aint updating shit!!" << std::endl;
}

void RigidShape::move(glm::vec2 moveVec) {
	std::cout << "I aint moving shit!!" << std::endl;
}

void RigidShape::rotate(float angle) {
	std::cout << "I aint rotating shit!!" << std::endl;
}

glm::vec2 RigidShape::getCenter() {
	return mCenter;
}

std::string RigidShape::getType() {
	std::cout << "I aint typing shit!!" << std::endl;
	std::string type = "error";
	return type;
}

float RigidShape::getInvMass() {
	return mInvMass;
}

glm::vec2 RigidShape::getVelocity() {
	return mVelocity;
}

float RigidShape::getAngularVelocity() {
	return mAngularVelocity;
}

float RigidShape::getRestitution() {
	return mRestitution;
}

float RigidShape::getFriction() {
	return mFriction;
}

float RigidShape::getInertia() {
	return mInertia;
}

float RigidShape::getRadius() {
	std::cout << "For circle" << std::endl;
	return 0.0f;
}

std::vector<glm::vec2> RigidShape::getFaceNormals() {
	std::cout << "for rectangle" << std::endl;
	std::vector<glm::vec2> empty;
	return empty;
}

std::vector<glm::vec2> RigidShape::getVertexes() {
	std::cout << "for rectangle" << std::endl;
	std::vector<glm::vec2> empty;
	return empty;
}

float RigidShape::getRotation() {
	std::cout << "I aint getting rotation" << std::endl;
	return 0;
}


void RigidShape::setAngularVelocity(float aV) {
	std::cout << "I aint getting angular velocity!!" << std::endl;
}

void RigidShape::setVelocity(glm::vec2 nV) {
	std::cout << "I aint setting velocity!!" << std::endl;
}