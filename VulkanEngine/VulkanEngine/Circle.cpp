#include "pch.h"
#include "Circle.h"
#include "CollisionInfo2D.h"
#include "VulkanManager.h"
#include "PhysicsManager.h"



void Circle::move(glm::vec2 moveVec)
{
	mStartPoint = vecMath.add(mStartPoint, moveVec);
	mCenter = vecMath.add(mCenter, moveVec);
}

std::string Circle::getType()
{
	return mType;
}

void Circle::rotate(float angle)
{
	mAngle += angle;
	mStartPoint = vecMath.rotate(mStartPoint, mCenter, angle);
}

void Circle::updateInertia()
{
	if (mInvMass == 0) 
	{
		mInertia = 0;
	}else{
		mInertia = (1 / mInvMass) * (mRadius * mRadius) / 12;
	}
}

void Circle::update()
{
	float dt = VulkanManager::GetInstance()->mUpdateIntervalInSeconds;
	mVelocity = vecMath.add(mVelocity, vecMath.scale(mAcceleration, dt));
	move(vecMath.scale(mVelocity, dt));
	mAngularVelocity += mAngularAcceleration * dt;
	rotate(mAngularVelocity * dt);
}

glm::vec2 Circle::getCenter() {
	return mCenter;
}

//float Rectangle::getInvMass() 
//{
	//return mInvMass;
//}

glm::vec2 Circle::getVelocity()
{
	return mVelocity;
}

float Circle::getAngularVelocity()
{
	return mAngularVelocity;
}

float Circle::getRestitution()
{
	return mRestitution;
}

float Circle::getFriction()
{
	return mFriction;
}

float Circle::getInertia()
{
	return mInertia;
}

float Circle::getRadius() 
{
	return mRadius;
}

float Circle::getRotation()
{
	return mAngle;
}

void Circle::setAngularVelocity(float aV)
{
	mAngularVelocity = aV;
}

void Circle::setVelocity(glm::vec2 nV)
{
	mVelocity = nV;
}