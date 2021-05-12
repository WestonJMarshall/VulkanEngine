#pragma once
#include "pch.h"
#include "RigidShape.h"
#include "CollisionInfo2D.h"
#include "2DVectorMath.h"
#include <string>
class Rectangle : public RigidShape 
{
private:
	std::string mType;
	float mWidth;
	float mHeight;
	float mBoundRadius;
	float mInvMass;
	bool mIsStatic;
	std::vector<glm::vec2> mVertex;
	std::vector<glm::vec2> mFaceNormal;
	glm::vec2 mRotation;
	VectorMath2D vecMath;

public:
	Rectangle(glm::vec2 center, float mass, float friction, float restitution, float width, float height) :RigidShape(center, mass, friction, restitution) {
		mType = "Rectangle";
		mWidth = width;
		mHeight = height;
		if (mass != 0)
		{
			mInvMass = 1 / mass;
		}
		else {
			mInvMass = 0;
		}
		
		mBoundRadius = glm::sqrt(width * width + height * height) / 2;
		updateInertia();

		mVertex.push_back(glm::vec2(center.x - width / 2, center.y - height / 2));
		mVertex.push_back(glm::vec2(center.x + width / 2, center.y - height / 2));
		mVertex.push_back(glm::vec2(center.x + width / 2, center.y + height / 2));
		mVertex.push_back(glm::vec2(center.x - width / 2, center.y + height / 2));


		mFaceNormal.push_back(glm::vec2(mVertex[1].x - mVertex[2].x, mVertex[1].y - mVertex[2].y));
		mFaceNormal[0] = normalize(mFaceNormal[0]);
		mFaceNormal.push_back(glm::vec2(mVertex[2].x - mVertex[3].x, mVertex[2].y - mVertex[3].y));
		mFaceNormal[1] = normalize(mFaceNormal[1]);
		mFaceNormal.push_back(glm::vec2(mVertex[3].x - mVertex[0].x, mVertex[3].y - mVertex[0].y));
		mFaceNormal[2] = normalize(mFaceNormal[2]);
		mFaceNormal.push_back(glm::vec2(mVertex[0].x - mVertex[1].x, mVertex[0].y - mVertex[1].y));
		mFaceNormal[3] = normalize(mFaceNormal[3]);
	};

	//drawing will be handled elsewhere
	void move(glm::vec2 moveVec) override;

	void rotate(float angle) override;

	void updateInertia();

	void update() override;

	std::string getType() override;

	std::vector<glm::vec2> getFaceNormals() override;

	std::vector<glm::vec2> getVertexes() override;

	float getRotation() override;

	void setAngularVelocity(float aV) override;

	void setVelocity(glm::vec2 nV) override;

};