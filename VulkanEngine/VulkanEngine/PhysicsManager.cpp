#include "pch.h"
#include "PhysicsManager.h"
#include "GameObject.h"
#include "GameManager.h"
#include "VulkanManager.h"
#include "CollisionInfo2D.h"
#include "2DVectorMath.h"

#define COLLISION_STEPS 4

#pragma region Singleton

PhysicsManager* PhysicsManager::instance = nullptr;
float oldTime = 0;

PhysicsManager* PhysicsManager::GetInstance()
{
    if (instance == nullptr) {
        instance = new PhysicsManager();
        instance->physicsObjects.resize(PhysicsLayers::PhysicsLayerCount);

        for (size_t i = 0; i < instance->physicsObjects.size(); i++) {
            instance->physicsObjects[i] = std::vector<std::shared_ptr<PhysicsObject>>();
        }
    }

    return instance;
}

#pragma endregion

#pragma region Accessors

float PhysicsManager::GetGravity()
{
    return gravity;
}

void PhysicsManager::SetGravity(float value)
{
    gravity = value;
}

glm::vec3 PhysicsManager::GetGravityDirection()
{
    return gravityDirection;
}

void PhysicsManager::SetGravityDirection(glm::vec3 value)
{
    gravityDirection = value;
}

int PhysicsManager::AddPhysicsObject(std::shared_ptr<Transform> transform, std::shared_ptr<Mesh> mesh, PhysicsLayers layer, ColliderTypes::ColliderTypes colliderType, float mass, bool affectedByGravity, bool alive)
{
    std::shared_ptr<PhysicsObject> newObject = std::make_shared<PhysicsObject>(transform, layer, colliderType, mass, affectedByGravity, alive);
    newObject->GetCollider()->GenerateFromMesh(mesh);

    int index = -1;

    for (int i = 0; i < physicsObjects[layer].size(); i++) {
        if (physicsObjects[layer][i] == nullptr) {
            physicsObjects[layer][i] = newObject;
            index = i;
        }
    }

    if (index == -1) {
        physicsObjects[layer].push_back(newObject);
        index = physicsObjects[layer].size() - 1;
    }

    return layer + index * 100;
}

void PhysicsManager::RemovePhysicsObject(int ID)
{
    PhysicsLayers layer = (PhysicsLayers)(ID % 100);
    int index = (ID / 100);

    physicsObjects[layer][index] = nullptr;
}

std::shared_ptr<PhysicsObject> PhysicsManager::GetPhysicsObject(int ID)
{
    PhysicsLayers layer = (PhysicsLayers)(ID % 100);
    int index = (ID / 100);

    return physicsObjects[layer][index];
}

#pragma endregion

#pragma region Update

void PhysicsManager::Update()
{
    //Check for collisions
    //DetectCollisions();
	DetectCollisions2D();
}

#pragma endregion

#pragma region Collision Detection

void PhysicsManager::DetectCollisions()
{
	/*
    CollisionData data = {};

    //Check Dynamic objects against all objects
		for (size_t i = 0; i < physicsObjects[PhysicsLayers::Dynamic].size(); i++) {
			if (physicsObjects[PhysicsLayers::Dynamic][i]->GetAlive()) {
				for (size_t j = i + 1; j < physicsObjects[PhysicsLayers::Dynamic].size(); j++) {
					if (physicsObjects[PhysicsLayers::Dynamic][j]->GetAlive()) {
						if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Dynamic][j], COLLISION_STEPS, data)) {
							ResolveCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Dynamic][j], data);
							physicsObjects[PhysicsLayers::Dynamic][j]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject());
							physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Dynamic][j]->GetGameObject());
						}
					}
				}

				for (size_t j = 0; j < physicsObjects[PhysicsLayers::Static].size(); j++) {
					if (physicsObjects[PhysicsLayers::Static][j]->GetAlive()) {
						if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Static][j], COLLISION_STEPS, data)) {
							ResolveCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Static][j], data);
							physicsObjects[PhysicsLayers::Static][j]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject());
							physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Static][j]->GetGameObject());
						}
					}
				}

				for (size_t j = 0; j < physicsObjects[PhysicsLayers::Trigger].size(); j++) {
					if (physicsObjects[PhysicsLayers::Trigger][j]->GetAlive()) {
						if (CheckCollision(physicsObjects[PhysicsLayers::Dynamic][i], physicsObjects[PhysicsLayers::Trigger][j], COLLISION_STEPS, data)) {
							physicsObjects[PhysicsLayers::Trigger][j]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject());
							physicsObjects[PhysicsLayers::Dynamic][i]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Trigger][j]->GetGameObject());
						}
					}
				}
			}
		}

		//Check Static objects against triggers
		for (size_t i = 0; i < physicsObjects[PhysicsLayers::Static].size(); i++) {
			if (physicsObjects[PhysicsLayers::Static][i]->GetAlive()) {
				for (size_t j = 0; j < physicsObjects[PhysicsLayers::Trigger].size(); j++) {
					if (physicsObjects[PhysicsLayers::Trigger][j]->GetAlive()) {
						if (CheckCollision(physicsObjects[PhysicsLayers::Static][i], physicsObjects[PhysicsLayers::Trigger][j], COLLISION_STEPS, data)) {
							physicsObjects[PhysicsLayers::Trigger][j]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Static][i]->GetGameObject());
							physicsObjects[PhysicsLayers::Static][i]->GetGameObject()->OnCollision(physicsObjects[PhysicsLayers::Trigger][j]->GetGameObject());
						}
					}
				}
			}
		}
	*/
}


void PhysicsManager::DetectCollisions2D() 
{
	//for (int i = 0; i < GameManager::GetInstance()->rigidShapes.size(); i++)
	//{
		//std::cout << GameManager::GetInstance()->rigidShapes[i]->getFaceNormals()[i].x << ", " << GameManager::GetInstance()->rigidShapes[i]->getFaceNormals()[i].y << std::endl;
	//}
	int relaxationCount = 1;
	//reset collisionInfo
	CollisionInfo2D collisionInfo = CollisionInfo2D();
	for (int k = 0; k < relaxationCount; k++) 
	{
		for (int i = 0; i < GameManager::GetInstance()->rigidShapes.size(); i++)
		{
			for (int j = i + 1; j < GameManager::GetInstance()->rigidShapes.size(); j++)
			{
				//std::cout << GameManager::GetInstance()->rigidShapes[i]->getCenter().x << ", " << GameManager::GetInstance()->rigidShapes[i]->getCenter().y << std::endl;
				//std::cout << GameManager::GetInstance()->rigidShapes[j]->getCenter().x << ", " << GameManager::GetInstance()->rigidShapes[j]->getCenter().y << std::endl;
				if (CollisionTest2D(GameManager::GetInstance()->rigidShapes[i], GameManager::GetInstance()->rigidShapes[j], collisionInfo)) 
				{
					//std::cout << "HIT!" << " objects: " << i << " and "<< j <<  std::endl;
					if (vecMath.dot(collisionInfo.getNormal(), vecMath.subtract(GameManager::GetInstance()->rigidShapes[j]->getCenter(), GameManager::GetInstance()->rigidShapes[i]->getCenter())) < 0 )
					{
						collisionInfo.changeDir();
					}

					ResolveVelocity(GameManager::GetInstance()->rigidShapes[i], GameManager::GetInstance()->rigidShapes[j], collisionInfo);
				}
			}
		}
	}
}

bool PhysicsManager::CollisionTest2D(std::shared_ptr<RigidShape> rect1, std::shared_ptr<RigidShape> rect2, CollisionInfo2D &collisionInfo)
{
	bool status = false;
	if (rect2->getType() == "Circle") {
		//do nothing for now
	}
	else {
		status = RectRectCollision2D(rect1, rect2, collisionInfo);
	}
	
	return status;
}

bool PhysicsManager::CheckCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, CollisionData& data)
{
    // If the 2 objects don't share a dimension::
    if (!physicsObject1->SharesDimension(physicsObject2))
        return false;

    //If one of the objects is a sphere do the sphere collider check
    if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {
        return CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject1->GetCollider()), physicsObject2->GetCollider(), data);
    }

    if (physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {
        return CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject2->GetCollider()), physicsObject1->GetCollider(), data);
    }

    //If both objects are AABB do the AABB check
    if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::AABB && physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::AABB) {
        if (CheckAABBCollision(std::static_pointer_cast<AABBCollider>(physicsObject1->GetCollider()), std::static_pointer_cast<AABBCollider>(physicsObject2->GetCollider()), data)) {
            physicsObject1->SetColliderColor(glm::vec3(1.0f, 0.0f, 0.0f));
            physicsObject2->SetColliderColor(glm::vec3(1.0f, 0.0f, 0.0f));
            return true;
        }
        return false;
    }

    //Otherwise do the SAT check
    return SAT(physicsObject1->GetCollider(), physicsObject2->GetCollider(), data);
}

bool PhysicsManager::CheckCollision(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2, int numSteps, CollisionData& data)
{
    // If the 2 objects don't share a dimension::
    if (!physicsObject1->SharesDimension(physicsObject2))
        return false;

    glm::vec3 preCheckPosition = physicsObject1->GetTransform()->GetPosition();
    //Iterate through the number of steps set
    for (float i = 1; i >= 0; i -= 1.0f / numSteps)
    {
        //First, we combine both objects velocity vectors together, meaning we act as though one of the objects is static
        glm::vec3 totalStepTranslation = -((physicsObject1->GetVelocity() * i) + (physicsObject2->GetVelocity() * i));

        physicsObject1->GetTransform()->SetPosition(physicsObject1->GetTransform()->GetPosition() + totalStepTranslation);

        //If one of the objects is a sphere do the sphere collider check
        if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {

            if (CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject1->GetCollider()), physicsObject2->GetCollider(), data))
            {
                physicsObject1->GetTransform()->SetPosition(preCheckPosition);
                return true;
            }
            physicsObject1->GetTransform()->SetPosition(preCheckPosition);
        }

        else if (physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::Sphere) {
            if (CheckSphereCollision(std::static_pointer_cast<SphereCollider>(physicsObject2->GetCollider()), physicsObject1->GetCollider(), data))
            {
                physicsObject1->GetTransform()->SetPosition(preCheckPosition);
                return true;
            }
            physicsObject1->GetTransform()->SetPosition(preCheckPosition);
        }

        //If both objects are AABB do the AABB check
        else if (physicsObject1->GetCollider()->GetColliderType() == ColliderTypes::AABB && physicsObject2->GetCollider()->GetColliderType() == ColliderTypes::AABB) {
            if (CheckAABBCollision(std::static_pointer_cast<AABBCollider>(physicsObject1->GetCollider()), std::static_pointer_cast<AABBCollider>(physicsObject2->GetCollider()), data)) {
                physicsObject1->SetColliderColor(glm::vec3(1.0f, 0.0f, 0.0f));
                physicsObject2->SetColliderColor(glm::vec3(1.0f, 0.0f, 0.0f));
                physicsObject1->GetTransform()->SetPosition(preCheckPosition);
                return true;
            }
            physicsObject1->GetTransform()->SetPosition(preCheckPosition);
        }

        //Otherwise do the SAT check
        else {
            if (SAT(physicsObject1->GetCollider(), physicsObject2->GetCollider(), data))
            {
                physicsObject1->GetTransform()->SetPosition(preCheckPosition);
                return true;
            }
            physicsObject1->GetTransform()->SetPosition(preCheckPosition);
        }
    }
    return false;
}

bool PhysicsManager::RectRectCollision2D(std::shared_ptr<RigidShape> rect1, std::shared_ptr<RigidShape> rect2, CollisionInfo2D &collisionInfo)
{
	bool status1 = false;
	bool status2 = false;

	CollisionInfo2D CollisionInfoR1 = CollisionInfo2D();
	CollisionInfo2D CollisionInfoR2 = CollisionInfo2D();

	status1 = findAxisLeastPenetration(rect1, rect2, CollisionInfoR1);
	if (status1) {
		status2 = findAxisLeastPenetration(rect2, rect1, CollisionInfoR2);
		if (status2) {
			if (CollisionInfoR1.getDepth() < CollisionInfoR2.getDepth()) {
				glm::vec2 depthVec = vecMath.scale(CollisionInfoR1.getNormal(), CollisionInfoR1.getDepth());
				collisionInfo.setInfo(CollisionInfoR1.getDepth(), CollisionInfoR1.getNormal(), vecMath.subtract(CollisionInfoR1.mStart, depthVec));
			}
			else {
				collisionInfo.setInfo(CollisionInfoR2.getDepth(), vecMath.scale(CollisionInfoR2.getNormal(), -1.0f), CollisionInfoR2.mStart);
			}
		}
	}
	return status1 && status2;
}

bool PhysicsManager::findAxisLeastPenetration(std::shared_ptr<RigidShape> thisShape, std::shared_ptr<RigidShape> otherShape, CollisionInfo2D &data) 
{
	glm::vec2 n;
	glm::vec2 supportPoint;
	float bestDistance = 999999;
	int bestIndex = 0;
	bool hasSupport = true;
	int i = 0;
	while (hasSupport && (i < thisShape->getFaceNormals().size())) 
	{
		n = thisShape->getFaceNormals()[i];
		glm::vec2 dir = vecMath.scale(n, -1.0f);
		glm::vec2 ptOnEdge = thisShape->getVertexes()[i];

		findSupportPoint(otherShape, dir, ptOnEdge);

		//if zero vector has no support
		hasSupport = (tmpSupportPoint != glm::vec2(0, 0));
		if (hasSupport && tmpDistance < bestDistance)
		{
			bestDistance = tmpDistance;
			bestIndex = i;
			supportPoint = tmpSupportPoint;
			
		}
		
		i += 1;
	}
	
	if (hasSupport) 
	{
		glm::vec2 bestVec = vecMath.scale(thisShape->getFaceNormals()[bestIndex], bestDistance);
		//std::cout << "moving by: " << bestVec.x << ", " << bestVec.y << " with a distance of: " << bestDistance << std::endl;
		data.setInfo(bestDistance, thisShape->getFaceNormals()[bestIndex], vecMath.add(supportPoint, bestVec));
	}
	
	
	return hasSupport;
}

void PhysicsManager::findSupportPoint(std::shared_ptr<RigidShape> r1, glm::vec2 dir, glm::vec2 ptOnEdge)
{
	glm::vec2 vToEdge;
	float projection;
	tmpDistance = -999999;
	tmpSupportPoint = glm::vec2(0, 0);
	hasSupportPoint = false;
	for (int i = 0; i < r1->getVertexes().size(); i++)
	{
		vToEdge = vecMath.subtract(r1->getVertexes()[i], ptOnEdge);
		projection = vecMath.dot(vToEdge, dir);
	
		if (projection > 0 && projection > tmpDistance) 
		{
			tmpSupportPoint = r1->getVertexes()[i];
			tmpDistance = projection;
		}
	}
}

bool PhysicsManager::SharesDimension(std::shared_ptr<PhysicsObject> physicsObject1, std::shared_ptr<PhysicsObject> physicsObject2)
{
    return false;
}

bool PhysicsManager::CheckSphereCollision(std::shared_ptr<SphereCollider> sphereCollider, std::shared_ptr<Collider> other, CollisionData& data)
{
    glm::vec3 closestPoint = other->ClosestToPoint(sphereCollider->GetTransform()->GetPosition());

    if (sphereCollider->ContainsPoint(closestPoint)) {
        data.collisionNormal = closestPoint - sphereCollider->GetTransform()->GetPosition();
        //Normalize collision normal
        float normalLength = glm::distance(closestPoint, sphereCollider->GetTransform()->GetPosition());
        data.collisionNormal = data.collisionNormal / normalLength;
        data.intersectionDistance = sphereCollider->getRadius() - normalLength;
        data.contactPoint = closestPoint + data.collisionNormal * data.intersectionDistance * 0.5f;
        return true;
    }

    return false;
}

bool PhysicsManager::CheckAABBCollision(std::shared_ptr<AABBCollider> collider1, std::shared_ptr<AABBCollider> collider2, CollisionData& data)
{
    if (collider1->GetTransform()->GetPosition().x + collider1->GetExtents().x < collider2->GetTransform()->GetPosition().x - collider2->GetExtents().x ||
        collider1->GetTransform()->GetPosition().x - collider1->GetExtents().x > collider2->GetTransform()->GetPosition().x + collider2->GetExtents().x ||
        collider1->GetTransform()->GetPosition().y + collider1->GetExtents().y < collider2->GetTransform()->GetPosition().y - collider2->GetExtents().y ||
        collider1->GetTransform()->GetPosition().y - collider1->GetExtents().y > collider2->GetTransform()->GetPosition().y + collider2->GetExtents().y || 
        collider1->GetTransform()->GetPosition().z + collider1->GetExtents().z < collider2->GetTransform()->GetPosition().z - collider2->GetExtents().z ||
        collider1->GetTransform()->GetPosition().z - collider1->GetExtents().z > collider2->GetTransform()->GetPosition().z + collider2->GetExtents().z) {
        return false;
    }

    glm::vec3 col1Min = collider1->GetTransform()->GetPosition() - collider1->GetExtents();
    glm::vec3 col1Max = collider1->GetTransform()->GetPosition() + collider1->GetExtents();
    glm::vec3 col2Min = collider2->GetTransform()->GetPosition() - collider2->GetExtents();
    glm::vec3 col2Max = collider2->GetTransform()->GetPosition() + collider2->GetExtents();

    glm::vec3 overlapStart = glm::vec3(glm::max(col1Min.x, col2Min.x), glm::max(col1Min.y, col2Min.y), glm::max(col1Min.z, col2Min.z));
    glm::vec3 overlapEnd = glm::vec3(glm::min(col1Max.x, col2Max.x), glm::min(col1Max.y, col2Max.y), glm::min(col1Max.z, col2Max.z));
    glm::vec3 overlap = overlapEnd - overlapStart;

    data = {};
    //Set contact point as the center of the overlap
    data.contactPoint = (overlapStart + overlapEnd) * 0.5f;
    //Set normal as the axis with the least overlap
    data.intersectionDistance = glm::min(overlap.x, glm::min(overlap.y, overlap.z));
    if (data.intersectionDistance == overlap.x) {
        data.collisionNormal = glm::vec3(1, 0, 0);
    }
    else if (data.intersectionDistance == overlap.y) {
        data.collisionNormal = glm::vec3(0, 1, 0);
    }
    else {
        data.collisionNormal = glm::vec3(0, 0, 1);
    }

    //Make sure collision normal points from object 1 to object 2
    if (glm::dot(data.collisionNormal, collider2->GetTransform()->GetPosition() - collider1->GetTransform()->GetPosition()) < 0) {
        data.collisionNormal *= -1;
    }

    return true;
}

bool PhysicsManager::SAT(std::shared_ptr<Collider> collider1, std::shared_ptr<Collider> collider2, CollisionData& data)
{
    std::vector<glm::vec3> SATaxis = std::vector<glm::vec3>();
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f));
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
    SATaxis.push_back(collider1->GetTransform()->GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f));
    SATaxis.push_back(collider2->GetTransform()->GetOrientation() * glm::vec3(0.0f, 0.0f, 1.0f));

    for (int i = 0; i < 3; i++) {
        for (int j = 3; j < 6; j++) {
            //if(glm::cross(SATaxis[i], SATaxis[j]) != glm::vec3(0.0f,0.0f,0.0f))
            SATaxis.push_back(glm::cross(SATaxis[i], SATaxis[j]));
        }
    }

    ProjectionData closestData[2] = {
        ProjectionData(),
        ProjectionData()
    };
    float minOverlap = -1;
    glm::vec3 closestAxis = SATaxis[0];
    float time = Time::GetTotalTime();
    for (int i = 0; i < SATaxis.size(); i++) {
        ProjectionData projection1 = collider1->ProjectOntoAxis(SATaxis[i]);
        ProjectionData projection2 = collider2->ProjectOntoAxis(SATaxis[i]);

        //Calculate overlap
        float overlap = glm::min(projection1.minMax.y, projection2.minMax.y) - glm::max(projection1.minMax.x, projection2.minMax.x);

        //Exit if there is no overlap
        if (overlap <= 0) {
            return false;
        }

        //Compare with min overlap and set min overlap if it hasn't been set yet
        if (minOverlap == -1 || overlap < minOverlap) {
            minOverlap = overlap;
            closestAxis = SATaxis[i];
            closestData[0] = projection1;
            closestData[1] = projection2;
        }
    }

    //Calculate collision data
    data.collisionNormal = closestAxis;

    //Ensure collision normal points from object 1 to object 2
    if (glm::dot(closestAxis, collider2->GetTransform()->GetPosition() - collider1->GetTransform()->GetPosition()) < 0) {
        data.collisionNormal *= -1;
    }

    data.intersectionDistance = minOverlap;

    // Figure out the type of contact
    std::vector<glm::vec3> collisionPoints[2]{
        std::vector<glm::vec3>(),
        std::vector<glm::vec3>()
    };

    if (closestData[0].minMax.x < closestData[1].minMax.x) {
        collisionPoints[0] = closestData[0].maxPoints;
        collisionPoints[1] = closestData[1].minPoints;
    }
    else {
        collisionPoints[0] = closestData[1].maxPoints;
        collisionPoints[1] = closestData[0].minPoints;
    }

    if (collisionPoints[0].size() == 1 && collisionPoints[1].size() == 4)
    {
        collisionPoints[1].resize(3);
    }
    else if (collisionPoints[1].size() == 1 && collisionPoints[0].size() == 4)
    {
        collisionPoints[0].resize(3);
    }

    switch (collisionPoints[0].size() + collisionPoints[1].size()) {
    case 2: //Point on Point collision
        data.contactPoint = (collisionPoints[0][0] + collisionPoints[1][0]) * 0.5f;
        break;
    case 3: //Point on Edge collision
    {
        glm::vec3 point;
        glm::vec3 edge[2];

        if (collisionPoints[0].size() == 1) {
            point = collisionPoints[0][0];
            edge[0] = collisionPoints[1][0];
            edge[1] = collisionPoints[1][1];
        }
        else {
            point = collisionPoints[1][0];
            edge[0] = collisionPoints[0][0];
            edge[1] = collisionPoints[0][1];
        }

        data.contactPoint = (point + ProjectPointOnEdge(point, edge, true)) * 0.5f;
        break;
    }
    case 4: //Edge on Edge or Point on Face collision
    {
        if (collisionPoints[0].size() == 2) { //Edge on Edge
            glm::vec3* projectedEdge1 = ProjectEdgeOnEdge(collisionPoints[0].data(), collisionPoints[1].data(), true);
            glm::vec3* projectedEdge2 = ProjectEdgeOnEdge(collisionPoints[1].data(), collisionPoints[0].data(), true);

            data.contactPoint = ((projectedEdge1[0] + projectedEdge1[1]) * 0.5f + (projectedEdge1[0] + projectedEdge1[1]) * 0.5f) * 0.5f;
        }
        else {
            glm::vec3 point;
            glm::vec3 face[3];

            if (collisionPoints[0].size() == 1) {
                point = collisionPoints[0][0];
                face[0] = collisionPoints[1][0];
                face[1] = collisionPoints[1][1];
                face[2] = collisionPoints[1][2];
            }
            else {
                point = collisionPoints[1][0];
                face[0] = collisionPoints[0][0];
                face[1] = collisionPoints[0][1];
                face[2] = collisionPoints[0][2];
            }

            data.contactPoint = (point + ProjectPointOnFace(point, face, true)) * 0.5f;
        }
        break;
    }
    case 5: //Edge on Face collision
    {
        glm::vec3 edge[2];
        glm::vec3 face[3];

        if (collisionPoints[0].size() == 2) {
            edge[0] = collisionPoints[0][0];
            edge[1] = collisionPoints[0][1];
            face[0] = collisionPoints[1][0];
            face[1] = collisionPoints[1][1];
            face[2] = collisionPoints[1][2];
        }
        else {
            edge[0] = collisionPoints[1][0];
            edge[1] = collisionPoints[1][1];
            face[0] = collisionPoints[0][0];
            face[1] = collisionPoints[0][1];
            face[2] = collisionPoints[0][2];
        }

        glm::vec3* projectedEdge = ProjectEdgeOnFace(edge, face, true);

        data.contactPoint = ((projectedEdge[0] + projectedEdge[1]) * 0.5f + (edge[0] + edge[1]) * 0.5f) * 0.5f;
        break;
    }
    default: //Face on Face collision
    {
        glm::vec3* projectedFace1;
        glm::vec3* projectedFace2;

        projectedFace1 = ProjectFaceOnFace(collisionPoints[0].data(), collisionPoints[1].data(), true);
        projectedFace2 = ProjectFaceOnFace(collisionPoints[1].data(), collisionPoints[0].data(), true);

        glm::vec3 midpoints[2] = {
            glm::vec3(0,0,0),
            glm::vec3(0,0,0)
        };
        for (int i = 0; i < 3; i++) {
            midpoints[0] += projectedFace1[i];
            midpoints[1] += projectedFace2[i];
        }

        midpoints[0] /= 3.0f;
        midpoints[1] /= 3.0f;

        data.contactPoint = (midpoints[0] + midpoints[1]) * 0.5f;
        break;
    }
    }

    return true;
}

#pragma endregion

#pragma region Collision Resolution

void PhysicsManager::ResolveCollision(std::shared_ptr<RigidShape> physicsObject1, std::shared_ptr<RigidShape> physicsObject2, CollisionInfo2D &data)
{
	float correctnessRate = 0.8;
	float s1InvMass = physicsObject1->getInvMass();
	float s2InvMass = physicsObject2->getInvMass();

	float num = data.getDepth() / (s1InvMass + s2InvMass) * correctnessRate;
	glm::vec2 correctionAmount = vecMath.scale(data.getNormal(), num);

	physicsObject1->move(vecMath.scale(correctionAmount, -s1InvMass));
	physicsObject2->move(vecMath.scale(correctionAmount, s2InvMass));
	
	//float invMass1 =  1 / physicsObject1->GetMass();
	//float invMass2 =  1 / physicsObject2->GetMass();

	//float num = data.intersectionDistance / (invMass1 + invMass2) * 0.8f;
	//glm::vec3 correctionAmount = data.collisionNormal * num;


    //Double Dynamic
    //if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic && physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
        //TODO: scale resolution movement based on velocity and mass
		//physicsObject1->GetTransform()->Translate(correctionAmount * -invMass1);
		//physicsObject2->GetTransform()->Translate(correctionAmount * invMass2);
		
		//physicsObject1->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * -0.4f);
        //physicsObject2->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * 0.4f);
    //}
    //else { //One Dynamic and One Static
        //Figure out which object is static and which is dynamic
        //if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
			//physicsObject1->GetTransform()->Translate(correctionAmount * -invMass1);
			//physicsObject1->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance * -1.0f);
       // }
        //else {
			//physicsObject2->GetTransform()->Translate(correctionAmount * invMass2);
			//physicsObject2->GetTransform()->Translate(data.collisionNormal * data.intersectionDistance);
        //}
    //}

    //Resolve Velocities after positions have been fixed
    //ResolveVelocity(physicsObject1, physicsObject2, data);

    //TODO: Call both object's on collision methods
}

void PhysicsManager::ResolveVelocity(std::shared_ptr<RigidShape> physicsObject1, std::shared_ptr<RigidShape> physicsObject2, CollisionInfo2D &data)
{
	if (physicsObject1->getInvMass() == 0 && physicsObject2->getInvMass() == 0) 
	{
		return;
	}

	//POSITIONAL CORRECTION FLAG
	if (true) 
	{
		ResolveCollision(physicsObject1, physicsObject2, data);
	}

	float invMass1 = physicsObject1->getInvMass();
	float invMass2 = physicsObject2->getInvMass();
	glm::vec2 center1 = physicsObject1->getCenter();
	glm::vec2 center2 = physicsObject2->getCenter();
	glm::vec2 velocity1 = physicsObject1->getVelocity();
	glm::vec2 velocity2 = physicsObject2->getVelocity();
	float angularVelocity1 = physicsObject1->getAngularVelocity();
	float angularVelocity2 = physicsObject2->getAngularVelocity();
	float restitution1 = physicsObject1->getRestitution();
	float restitution2 = physicsObject2->getRestitution();
	float friction1 = physicsObject1->getFriction();
	float friction2 = physicsObject2->getFriction();
	float inertia1 = physicsObject1->getInertia();
	float inertia2 = physicsObject2->getInertia();


	glm::vec2 n = data.getNormal(); //

	glm::vec2 start = vecMath.scale(data.mStart, (invMass2 / (invMass1 + invMass2)));
	glm::vec2 end = vecMath.scale(data.mStart, (invMass1 / (invMass1 + invMass2)));

	glm::vec2 p = vecMath.add(start, end);

	glm::vec2 r1 = vecMath.subtract(p, center1);
	glm::vec2 r2 = vecMath.subtract(p, center2);

	glm::vec2 v1 = vecMath.add(velocity1, glm::vec2(-1 * angularVelocity1 * r1.y, angularVelocity1 * r1.x));
	glm::vec2 v2 = vecMath.add(velocity2, glm::vec2(-1 * angularVelocity2 * r2.y, angularVelocity2 * r2.x));

	glm::vec2 relativeVelocity = vecMath.subtract(v2, v1);

	float rVelocityInNormal = vecMath.dot(relativeVelocity, n);

	if (rVelocityInNormal > 0) 
	{
		return;
	}

	float newRestitution = glm::min(restitution1, restitution2);
	float newFriction = glm::min(friction1, friction2);

	float R1crossN = vecMath.cross(r1, n);
	float R2crossN = vecMath.cross(r2, n);

	float jN = -(1 + newRestitution) * rVelocityInNormal;
	jN = jN / (invMass1 + invMass2 + R1crossN * R1crossN * inertia1 + R2crossN * R2crossN * inertia2);

	physicsObject1->setAngularVelocity(angularVelocity1 - (R1crossN * jN * inertia1));
	physicsObject2->setAngularVelocity(angularVelocity2 + (R2crossN * jN * inertia2));
	angularVelocity1 = physicsObject1->getAngularVelocity();
	angularVelocity2 = physicsObject2->getAngularVelocity();


	glm::vec2 impulse = vecMath.scale(n, jN);

	physicsObject1->setVelocity(vecMath.subtract(velocity1, vecMath.scale(impulse, invMass1)));
	physicsObject2->setVelocity(vecMath.add(velocity2, vecMath.scale(impulse, invMass2)));
	velocity1 = physicsObject1->getVelocity();
	velocity2 = physicsObject2->getVelocity();

	glm::vec2 tangent = vecMath.subtract(relativeVelocity, vecMath.scale(n, vecMath.dot(relativeVelocity, n)));

	tangent = vecMath.scale(vecMath.normalize(tangent), -1);

	float R1crossT = vecMath.cross(r1, tangent);
	float R2crossT = vecMath.cross(r2, tangent);

	float jT = -(1 + newRestitution) * vecMath.dot(relativeVelocity, tangent) * newFriction;
	jT = jT / (invMass1 + invMass2 + R1crossT * R1crossT * inertia1 + R2crossT * R2crossT * inertia2);

	if (jT > jN) {
		jT = jN;
	}

	impulse = vecMath.scale(tangent, jT);

	physicsObject1->setVelocity(vecMath.subtract(velocity1, vecMath.scale(impulse, invMass1)));
	physicsObject2->setVelocity(vecMath.add(velocity2, vecMath.scale(impulse, invMass2)));

	physicsObject1->setAngularVelocity(angularVelocity1 - R1crossT * jT * inertia1);
	physicsObject2->setAngularVelocity(angularVelocity2 + R2crossT * jT * inertia2);


	//float projectionMult[2];
	//projectionMult[0] = glm::dot(physicsObject1->GetVelocityAtPoint(data.contactPoint), data.collisionNormal) / glm::dot(data.collisionNormal, data.collisionNormal);
	//projectionMult[1] = glm::dot(physicsObject2->GetVelocityAtPoint(data.contactPoint), data.collisionNormal) / glm::dot(data.collisionNormal, data.collisionNormal);

	//glm::vec3 force[2];

	//float scaledMass1 = physicsObject1->GetMass() / 10;
	//float scaledMass2 = physicsObject2->GetMass() / 10;

	//glm::vec3 test2 = (data.collisionNormal * projectionMult[0] * -scaledMass2) / (float)(VulkanManager::GetInstance()->dt);
	//std::cout << "force of old: " << test2.x << ", " << test2.y << std::endl;
	//if (physicsObject1->GetPhysicsLayer() == PhysicsLayers::Dynamic && projectionMult[0] > 0) {
		//dont apply drag if object is on top
		//force[0] = (data.collisionNormal * projectionMult[0] * -scaledMass2) / (float)(VulkanManager::GetInstance()->dt);
	   // physicsObject1->ApplyForce(force[0], data.contactPoint, false);

		//if (physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic) {
			//force[0] = (data.collisionNormal * projectionMult[0] * -scaledMass1) / (float)(VulkanManager::GetInstance()->dt);
			//physicsObject2->ApplyForce(-force[0], data.contactPoint, false);
	   // }
   // }

   // if (physicsObject2->GetPhysicsLayer() == PhysicsLayers::Dynamic && projectionMult[1] < 0) {
		//if obj1 is moving, and obj2 is not, apply a "drag" to obj2
		//force[1] = (data.collisionNormal * projectionMult[1] * -scaledMass1) / (float)(VulkanManager::GetInstance()->dt);
		//if obj1 is moving and obj2 is not, apply a "drag" force to obj2
		//if (physicsObject1->GetVelocity().x != 0 && physicsObject2->GetVelocity().x == 0.0f) {
			//force[1].x = physicsObject1->GetVelocity().x * 50.0f;
		//}

		//physicsObject2->ApplyForce(force[1], data.contactPoint, false);
		//physicsObject1->ApplyForce(-force[1], data.contactPoint, false);
		//apply force to obj1 if it is moving only
		//if (physicsObject2->GetVelocity().x > 0) {
			//physicsObject1->ApplyForce(-force[1], data.contactPoint, false);
		//}

	//}
}

#pragma endregion

#pragma region Helper Functions

glm::vec3 PhysicsManager::ProjectPointOnEdge(glm::vec3 point, glm::vec3 edge[2], bool sizeClamp)
{
    glm::vec3 direction = point - edge[0];
    glm::vec3 edgeDirection = edge[1] - edge[0];
    float projectionMult = glm::dot(edgeDirection, direction) / glm::dot(edgeDirection, edgeDirection);

    if (sizeClamp) {
        projectionMult = glm::clamp(projectionMult, 0.0f, 1.0f);
    }

    return edge[0] + projectionMult * edgeDirection;
}

glm::vec3 PhysicsManager::ProjectPointOnFace(glm::vec3 point, glm::vec3 face[3], bool sizeClamp)
{
    glm::vec3 axis[2];
    axis[0] = face[1] - face[0];
    axis[1] = face[2] - face[0];

    glm::vec3 direction = point - face[0];

    float projectionMult[2];
    projectionMult[0] = glm::dot(axis[0], direction) / glm::dot(axis[0], axis[0]);
    projectionMult[1] = glm::dot(axis[1], direction) / glm::dot(axis[1], axis[1]);

    if (sizeClamp) {
        projectionMult[0] = glm::clamp(projectionMult[0], 0.0f, 1.0f);
        projectionMult[1] = glm::clamp(projectionMult[1], 0.0f, 1.0f);
    }

    return face[0] + axis[0] * projectionMult[0] + axis[1] * projectionMult[1];
}

glm::vec3* PhysicsManager::ProjectEdgeOnEdge(glm::vec3 edge1[2], glm::vec3 edge2[2], bool sizeClamp)
{
    glm::vec3 projectedEdge[2];

    projectedEdge[0] = ProjectPointOnEdge(edge1[0], edge2, sizeClamp);
    projectedEdge[1] = ProjectPointOnEdge(edge1[1], edge2, sizeClamp);

    return projectedEdge;
}

glm::vec3* PhysicsManager::ProjectEdgeOnFace(glm::vec3 edge[2], glm::vec3 face[3], bool sizeClamp)
{
    glm::vec3 projectedEdge[2];

    projectedEdge[0] = ProjectPointOnFace(edge[0], face, sizeClamp);
    projectedEdge[1] = ProjectPointOnFace(edge[1], face, sizeClamp);

    return projectedEdge;
}

glm::vec3* PhysicsManager::ProjectFaceOnFace(glm::vec3 face1[3], glm::vec3 face2[3], bool sizeClamp)
{
    glm::vec3 projectedFace[3];

    for (int i = 0; i < 3; i++) {
        projectedFace[i] = ProjectPointOnFace(face1[0], face2, sizeClamp);
    }

    return projectedFace;
}

#pragma endregion