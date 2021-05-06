#include "pch.h"
#include "Rectangle.h"
#include "CollisionInfo2D.h"
#include "VulkanManager.h"
#include "PhysicsManager.h"

std::string Rectangle::getType()
{
	return mType;
}


void Rectangle::move(glm::vec2 moveVec)
{
	for (int i = 0; i < mVertex.size(); i++)
	{
		mVertex[i] = vecMath.add(mVertex[i], moveVec);
	}
	mCenter = vecMath.add(mCenter, moveVec);
}

void Rectangle::rotate(float angle) 
{
	mAngle += angle / VulkanManager::GetInstance()->mUpdateIntervalInSeconds - (angle * 2);
	std::cout << mAngle << std::endl;
	for (int i = 0; i < mVertex.size(); i++)
	{
		mVertex[i] = vecMath.rotate(mVertex[i], mCenter, angle);
	}

	mFaceNormal[0] = vecMath.subtract(mVertex[1], mVertex[2]);
	mFaceNormal[0] = glm::normalize(mFaceNormal[0]);
	mFaceNormal[1] = vecMath.subtract(mVertex[2], mVertex[3]);
	mFaceNormal[1] = glm::normalize(mFaceNormal[1]);
	mFaceNormal[2] = vecMath.subtract(mVertex[3], mVertex[0]);
	mFaceNormal[2] = glm::normalize(mFaceNormal[2]);
	mFaceNormal[3] = vecMath.subtract(mVertex[0], mVertex[1]);
	mFaceNormal[3] = glm::normalize(mFaceNormal[3]);
}

void Rectangle::updateInertia() 
{
	if (mInvMass == 0) {
		mInertia = 0;
	}
	else {
		mInertia = (1 / mInvMass) * (mWidth * mWidth + mHeight * mHeight) / 12;
		mInertia = 1 / this->mInertia;
	}
}

void Rectangle::update() 
{
	float dt = VulkanManager::GetInstance()->mUpdateIntervalInSeconds;
	mVelocity = vecMath.add(mVelocity, vecMath.scale(mAcceleration, dt));
	move(vecMath.scale(mVelocity, dt));
	mAngularVelocity += mAngularAcceleration * dt;
	rotate(mAngularVelocity * dt);
}

glm::vec2 Rectangle::getCenter() {
	return mCenter;
}

//float Rectangle::getInvMass() 
//{
	//return mInvMass;
//}

glm::vec2 Rectangle::getVelocity() 
{
	return mVelocity;
}

float Rectangle::getAngularVelocity()
{
	return mAngularVelocity;
}

float Rectangle::getRestitution() 
{
	return mRestitution;
}

float Rectangle::getFriction() 
{
	return mFriction;
}

float Rectangle::getInertia()
{
	return mInertia;
}

std::vector<glm::vec2> Rectangle::getFaceNormals() 
{
	return mFaceNormal;
}

std::vector<glm::vec2> Rectangle::getVertexes()
{
	return mVertex;
}

float Rectangle::getRotation() 
{
	return mAngle;
}

void Rectangle::setAngularVelocity(float aV) 
{
	mAngularVelocity = aV;
}

void Rectangle::setVelocity(glm::vec2 nV) 
{
	mVelocity = nV;
}



