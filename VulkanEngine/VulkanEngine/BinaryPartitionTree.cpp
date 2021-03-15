#include "pch.h"
#include "BinaryPartitionTree.h"

#define OBJECT_COLLIDER(object) (object.lock()->GetPhysicsObject()->GetCollider())
#define OBJECT_PHYSICS(object) (object.lock()->GetPhysicsObject())

#define VEC3ZERO glm::vec3(0.0f, 0.0f, 0.0f)
#define VEC3MIN glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min())

BinaryPartitionTree::BinaryPartitionTree()
{
	objects = std::vector<std::weak_ptr<GameObject>>();
}

int BinaryPartitionTree::Initialize(int maxSubdivisions, int minToSubdivide)
{
	splitType = BSPSplitDirection::X;

	this->maxSubdivisions = maxSubdivisions;
	this->minToSubdivide = minToSubdivide;
	subdivisionLevel = 0;
	indexCount = std::make_shared<int>(0);
	index = 0;
	meanContentsValue = 0.0f;

	children.child1 = nullptr;
	children.child2 = nullptr;

	//FUTURE: ADD AARB SUPPORT
	bounds = std::make_shared<PhysicsObject>(
			std::make_shared<Transform>(glm::vec3(0.0f, 0.0f, 0.0f)),
			PhysicsLayers::Trigger,
			ColliderTypes::AABB,
			0.0f,
			false,
			true);

	bounds->AddDimension(0);

	initialized = true;

	//Remove children and parents if there are any
	BSP_Clear_Node();
	BSP_Clear_Resizing();

	return 1;
}

int BinaryPartitionTree::Fill(std::shared_ptr<GameObject> gameObject)
{
	if (!initialized) return 0;

	//Add the instances to the bsp, resize, then do nothing else
	objects.push_back(gameObject);
	BSP_Basic_Resize(gameObject);

	gameObject->GetPhysicsObject()->AddDimension(index);

	return 1;
}

int BinaryPartitionTree::Generate()
{
	if (!initialized) return 0;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> childGameObjects = std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>(new std::vector<std::shared_ptr<GameObject>>());
	BSP_Collect_Children(childGameObjects);

	for (auto& i : objects)
	{
		childGameObjects->push_back(i.lock());
	}

	//Remove all instance data in this node and below. Now all instances will be stored in 'childInstances' alone
	BSP_Clear_Node();

	//Put all the collected instances in the cleared node, then split this node into children as neccessary
	for (auto& i : *childGameObjects)
	{
		objects.push_back(i);
	}
	BSP_Distribute_Down();

	return 1;
}

int BinaryPartitionTree::Insert(std::shared_ptr<GameObject> gameObject)
{
	return 0;
}

int BinaryPartitionTree::Draw()
{
	return 1;
}

void Down_X(BinaryPartitionTree* bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		//FIX AXIS?
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(1.0f, 0.0f, 0.0f));
		bsp->SetMeansContentsValue(bsp->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}
void Down_Y(BinaryPartitionTree* bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(0.0f, 1.0f, 0.0f));
		bsp->SetMeansContentsValue(bsp->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}
void Down_Z(BinaryPartitionTree* bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(0.0f, 0.0f, 1.0f));
		bsp->SetMeansContentsValue(bsp->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}

void(*distributionFunction[3])(BinaryPartitionTree* bsp) =
{
	Down_X,
	Down_Y,
	Down_Z
};

void BinaryPartitionTree::BSP_Distribute_Down()
{
	//Average the position values of this node's instances,
	//so this node will split in the middle of all its instances along the appropriate axis
	distributionFunction[(int)splitType](this);

	//Subdivide
	if (objects.size() > minToSubdivide && subdivisionLevel < maxSubdivisions)
	{
		BSP_Generate_Children();

		for (auto& i : objects)
		{
			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			CollisionData data = {};

			OBJECT_PHYSICS(i)->AddDimension(children.child1->index);
			OBJECT_PHYSICS(i)->AddDimension(children.child2->index);

			if (PhysicsManager::GetInstance()->CheckCollision(OBJECT_PHYSICS(i), children.child1->bounds, data))
			{
				children.child1->Fill(i.lock());
				i.lock()->GetPhysicsObject()->RemoveDimension(index);
			}
			else
			{
				OBJECT_PHYSICS(i)->RemoveDimension(children.child1->index);
			}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			if(PhysicsManager::GetInstance()->CheckCollision(OBJECT_PHYSICS(i), children.child2->bounds, data))
			{
				children.child2->Fill(i.lock());
				i.lock()->GetPhysicsObject()->RemoveDimension(index);
			}
			else
			{
				OBJECT_PHYSICS(i)->RemoveDimension(children.child2->index);
			}
		}

		objects.clear();

		children.child1->BSP_Distribute_Down();
		children.child2->BSP_Distribute_Down();
	}
}

void BinaryPartitionTree::BSP_Basic_Resize(std::shared_ptr<GameObject> gameObject)
{
	//If this is the head node and the instance is outside of its bounds, make it bigger
	if (subdivisionLevel == 0)
	{
		Collider* bspColliderTemp = &*bounds->GetCollider();
		AABBCollider* bspCollider = static_cast<AABBCollider*>(bspColliderTemp);

		Collider* objectColliderTemp = &*gameObject->GetPhysicsObject()->GetCollider();
		AABBCollider* objectCollider = static_cast<AABBCollider*>(objectColliderTemp);

		glm::vec3 bspPosition = bspCollider->GetTransform()->GetPosition();
		glm::vec3 objectPosition = objectCollider->GetTransform()->GetPosition();

		glm::vec3 bspExtents = bspCollider->GetExtents();
		glm::vec3 objectExtents = objectCollider->GetExtents();

		if (objects.size() == 1)
		{
			SetBounds(objectPosition, objectExtents);
			return;
		}

		if (bspPosition.x + bspExtents.x < objectPosition.x + objectExtents.x)
		{
			float deltaX = glm::abs((bspPosition.x + bspExtents.x) - (objectPosition.x + objectExtents.x)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x + (deltaX / 2.0f), bspPosition.y, bspPosition.z),
				glm::vec3(bspExtents.x + deltaX, bspExtents.y, bspExtents.z));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
		if (bspPosition.x - bspExtents.x > objectPosition.x - objectExtents.x)
		{
			float deltaX = glm::abs((bspPosition.x - bspExtents.x) - (objectPosition.x - objectExtents.x)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x + (deltaX / 2.0f), bspPosition.y, bspPosition.z),
				glm::vec3(bspExtents.x + deltaX, bspExtents.y, bspExtents.z));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
		if (bspPosition.y + bspExtents.y < objectPosition.y + objectExtents.y)
		{
			float deltaY = glm::abs((bspPosition.y + bspExtents.y) - (objectPosition.y + objectExtents.y)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x, bspPosition.y + (deltaY / 2.0f), bspPosition.z),
				glm::vec3(bspExtents.x, bspExtents.y + deltaY, bspExtents.z));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
		if (bspPosition.y - bspExtents.y > objectPosition.y - objectExtents.y)
		{
			float deltaY = glm::abs((bspPosition.y - bspExtents.y) - (objectPosition.y - objectExtents.y)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x, bspPosition.y + (deltaY / 2.0f), bspPosition.z),
				glm::vec3(bspExtents.x, bspExtents.y + deltaY, bspExtents.z));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
		if (bspPosition.z + bspExtents.z < objectPosition.z + objectExtents.z)
		{
			float deltaZ = glm::abs((bspPosition.z + bspExtents.z) - (objectPosition.z + objectExtents.z)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x, bspPosition.y, bspPosition.z + (deltaZ / 2.0f)),
				glm::vec3(bspExtents.x, bspExtents.y, bspExtents.z + deltaZ));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
		if (bspPosition.z - bspExtents.z > objectPosition.z - objectExtents.z)
		{
			float deltaZ = glm::abs((bspPosition.z - bspExtents.z) - (objectPosition.z - objectExtents.z)) + std::numeric_limits<float>().epsilon();
			SetBounds(
				glm::vec3(bspPosition.x, bspPosition.y, bspPosition.z + (deltaZ / 2.0f)),
				glm::vec3(bspExtents.x, bspExtents.y, bspExtents.z + deltaZ));
			bspPosition = bspCollider->GetTransform()->GetPosition();
			bspExtents = bspCollider->GetExtents();
		}
	}
}

void BinaryPartitionTree::BSP_Collect_Children(std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> childGameObjects)
{
	if (children.child1 != nullptr)
	{
		//while child1 exits, loop through the child and get all the instances
		//and add them to the childInstances vector
		for (auto& i : children.child1->objects)
		{
			childGameObjects->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		children.child1->BSP_Collect_Children(childGameObjects);

		//do the same for child2 as above
		for (auto& i : children.child2->objects)
		{
			childGameObjects->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		children.child2->BSP_Collect_Children(childGameObjects);
	}
}

void BinaryPartitionTree::BSP_Generate_Children()
{
	float splitLoc = meanContentsValue / objects.size();

	Collider* bspColliderTemp = &*bounds->GetCollider();
	AABBCollider* bspCollider = static_cast<AABBCollider*>(bspColliderTemp);

	glm::vec3 bspPosition = bspCollider->GetTransform()->GetPosition();
	glm::vec3 bspExtents = bspCollider->GetExtents();

	std::shared_ptr<BinaryPartitionTree> child1 = std::make_shared<BinaryPartitionTree>();
	child1->Initialize(maxSubdivisions, minToSubdivide);
	child1->parent = std::make_shared<BinaryPartitionTree>(*this);
	child1->subdivisionLevel = subdivisionLevel + 1;
	child1->indexCount = indexCount;
	++*child1->indexCount.get();
	child1->index = *indexCount.get();
	child1->bounds->AddDimension(child1->index);

	std::shared_ptr<BinaryPartitionTree> child2 = std::make_shared<BinaryPartitionTree>();
	child2->Initialize(maxSubdivisions, minToSubdivide);
	child2->parent = std::make_shared<BinaryPartitionTree>(*this);
	child2->subdivisionLevel = subdivisionLevel + 1;
	child2->indexCount = indexCount;
	++*child2->indexCount.get();
	child2->index = *indexCount.get();
	child2->bounds->AddDimension(child2->index);

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->FrontBack->TopBottom
	if (splitType == BSPSplitDirection::X)
	{
		child1->splitType = BSPSplitDirection::Y;
		child2->splitType = BSPSplitDirection::Y;

		float xPos = splitLoc + (((bspPosition.x + bspExtents.x) - splitLoc) / 2.0f);
		float xExtent = bspExtents.x - xPos;

		child1->SetBounds(
			glm::vec3(xPos, bspPosition.y, bspPosition.z),
			glm::vec3(xExtent, bspExtents.y, bspExtents.z));

		xPos = splitLoc + (((bspPosition.x - bspExtents.x) - splitLoc) / 2.0f);
		xExtent = splitLoc - xPos;

		child2->SetBounds(
			glm::vec3(xPos, bspPosition.y, bspPosition.z),
			glm::vec3(xExtent, bspExtents.y, bspExtents.z));
	}
	else if (splitType == BSPSplitDirection::Y)
	{
		child1->splitType = BSPSplitDirection::Z;
		child2->splitType = BSPSplitDirection::Z;

		float yPos = splitLoc + (((bspPosition.y + bspExtents.y) - splitLoc) / 2.0f);
		float yExtent = bspExtents.y - yPos;

		child1->SetBounds(
			glm::vec3(bspPosition.x, yPos, bspPosition.z),
			glm::vec3(bspExtents.x, yExtent, bspExtents.z));

		yPos = splitLoc + (((bspPosition.y - bspExtents.y) - splitLoc) / 2.0f);
		yExtent = splitLoc - yPos;

		child2->SetBounds(
			glm::vec3(bspPosition.x, yPos, bspPosition.z),
			glm::vec3(bspExtents.x, yExtent, bspExtents.z));
	}
	else
	{
		child1->splitType = BSPSplitDirection::X;
		child2->splitType = BSPSplitDirection::X;

		float zPos = splitLoc + (((bspPosition.z + bspExtents.z) - splitLoc) / 2.0f);
		float zExtent = bspExtents.z - zPos;

		child1->SetBounds(
			glm::vec3(bspPosition.x, bspPosition.y, zPos),
			glm::vec3(bspExtents.x, bspExtents.y, zExtent));

		zPos = splitLoc + (((bspPosition.z - bspExtents.z) - splitLoc) / 2.0f);
		zExtent = splitLoc - zPos;

		child2->SetBounds(
			glm::vec3(bspPosition.x, bspPosition.y, zPos),
			glm::vec3(bspExtents.x, bspExtents.y, zExtent));
	}

	children.child1 = child1;
	children.child2 = child2;
}

void BinaryPartitionTree::BSP_Clear_Node(bool root)
{
	if (!initialized) return;

	//Find children
	if (children.child1 != nullptr)
	{
		BSP_Clear_Node_Specific(children.child1, false);
		BSP_Clear_Node_Specific(children.child2, false);

		children.child1 = nullptr;
		children.child2 = nullptr;
	}

	//Remove instances in root node
	objects.clear();
}

void BinaryPartitionTree::BSP_Clear_Node_Specific(std::shared_ptr<BinaryPartitionTree> bsp, bool root)
{
	if (!bsp->initialized) return;

	//Find children
	if (bsp->children.child1 != nullptr)
	{
		BSP_Clear_Node_Specific(bsp->children.child1, false);
		BSP_Clear_Node_Specific(bsp->children.child2, false);

		bsp->children.child1 = nullptr;
		bsp->children.child2 = nullptr;
	}
}

void BinaryPartitionTree::BSP_Clear_Resizing()
{
	SetBounds(VEC3MIN, VEC3MIN);
}

void BinaryPartitionTree::SetBounds(glm::vec3 center, glm::vec3 extents)
{
	//FIX
	bounds->GetCollider()->GetTransform()->SetPosition(center);
	Collider* colliderTemp = &*bounds->GetCollider();
	static_cast<AABBCollider*>(colliderTemp)->SetExtents(extents);
}

void BinaryPartitionTree::BSP_Place_Into(std::shared_ptr<GameObject> gameObject)
{
}
