#include "pch.h"
#include "BinaryPartitionTree.h"

#define OBJECT_COLLIDER(object) (object.lock()->GetPhysicsObject()->GetCollider())
#define OBJECT_PHYSICS(object) (object.lock()->GetPhysicsObject())

////Public

BinaryPartitionTree::BinaryPartitionTree()
{

}

int BinaryPartitionTree::Initialize(int maxSubdivisions, int minToSubdivide)
{
	splitType = BSPSplitDirection::X;

	maxSubdivisions = maxSubdivisions;
	minToSubdivide = minToSubdivide;
	subdivisionLevel = 0;
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

	return 1;
}

int BinaryPartitionTree::Generate()
{
	if (!initialized) return 0;

	//Take all other instances below this node and add it to this one, then delete all child nodes
	std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> childGameObjects;
	BSP_Collect_Children(childGameObjects);

	for (auto& i : objects)
	{
		childGameObjects.get()->push_back(i.lock());
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

int BinaryPartitionTree::Draw()
{
	return 1;
}

////Private

void Down_X(std::shared_ptr<BinaryPartitionTree> bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		//FIX AXIS?
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(1.0f, 0.0f, 0.0f));
		bsp.get()->SetMeansContentsValue(bsp.get()->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}
void Down_Y(std::shared_ptr<BinaryPartitionTree> bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(0.0f, 1.0f, 0.0f));
		bsp.get()->SetMeansContentsValue(bsp.get()->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}
void Down_Z(std::shared_ptr<BinaryPartitionTree> bsp)
{
	for (auto& i : bsp->GetObjects())
	{
		ProjectionData data = OBJECT_COLLIDER(i)->ProjectOntoAxis(glm::vec3(0.0f, 0.0f, 1.0f));
		bsp.get()->SetMeansContentsValue(bsp.get()->GetMeansContentsValue() + ((data.minMax.x + data.minMax.y) / 2.0f));
	}
}

void(*distributionFunction[3])(std::shared_ptr<BinaryPartitionTree> bsp) =
{
	Down_X,
	Down_Y,
	Down_Z
};

void BinaryPartitionTree::BSP_Distribute_Down()
{
	//Average the position values of this node's instances,
	//so this node will split in the middle of all its instances along the appropriate axis
	distributionFunction[(int)splitType](std::make_shared<BinaryPartitionTree>(*this));

	//Subdivide
	if (objects.size() > minToSubdivide && subdivisionLevel < maxSubdivisions)
	{
		BSP_Generate_Children();

		for (auto& i : objects)
		{
			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			CollisionData data = {};

			if (PhysicsManager::GetInstance()->CheckCollision(OBJECT_PHYSICS(i), children.child1->bounds, data))
			{
				children.child1->Fill(i.lock());
			}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			else if(PhysicsManager::GetInstance()->CheckCollision(OBJECT_PHYSICS(i), children.child2->bounds, data))
			{
				children.child2->Fill(i.lock());
			}
		}

		objects.clear();

		BSP_Distribute_Down_Specific(children.child1);
		BSP_Distribute_Down_Specific(children.child2);
	}
}

void BinaryPartitionTree::BSP_Distribute_Down_Specific(std::shared_ptr<BinaryPartitionTree> bsp)
{
	//Average the position values of this node's instances,
	//so this node will split in the middle of all its instances along the appropriate axis
	distributionFunction[(int)bsp.get()->splitType](bsp);

	//Subdivide
	if (bsp.get()->objects.size() > bsp.get()->minToSubdivide && bsp.get()->subdivisionLevel < bsp.get()->maxSubdivisions)
	{
		BSP_Generate_Children_Specific(bsp);

		for (auto& i : objects)
		{
			//FIX

			//if the bsp's child1 collides with the bounding box if the instance
			//add the instance to the child bsp, and removed from parent bsp
			//if (Bounding_Box_Collision(i->boundingBox, bsp->children.child1->boundingBox))
			//{
			//	BSP_Fill(i);
			//}
			//if the instance does not collide with the child1 bsp, assume the
			//instance collides with the child2 bsp
			//else
			//{
			//	BSP_Fill(i);
			//}
		}

		bsp.get()->objects.clear();

		BSP_Distribute_Down_Specific(bsp.get()->children.child1);
		BSP_Distribute_Down_Specific(bsp.get()->children.child2);
	}
}

void BinaryPartitionTree::BSP_Basic_Resize(std::shared_ptr<GameObject> gameObject)
{
	//WIP !!!!!!!!!!!!!!!!!!!!!!! WIP

	//If this is the head node and the instance is outside of its bounds, make it bigger
	if (subdivisionLevel == 0)
	{
		//#define RESIZE(dimension, operator) \
		//	if (gameObject.get()->GetTransform().get()->GetPosition().dimension < bsp->boundingBox.left) { bsp->boundingBox.left = instance->boundingBox.left - std::numeric_limits<float>::epsilon(); }
		//
		//
		//if (gameObject.get()->GetTransform().get()->GetPosition().x < bsp->boundingBox.left) { bsp->boundingBox.left = instance->boundingBox.left - std::numeric_limits<float>::epsilon(); }
		//if (gameObject.boundingBox.right > bsp->boundingBox.right) { bsp->boundingBox.right = instance->boundingBox.right + std::numeric_limits<float>::epsilon(); }
		//if (gameObject.boundingBox.bottom < bsp->boundingBox.bottom) { bsp->boundingBox.bottom = instance->boundingBox.bottom - std::numeric_limits<float>::epsilon(); }
		//if (gameObject.boundingBox.top > bsp->boundingBox.top) { bsp->boundingBox.top = instance->boundingBox.top + std::numeric_limits<float>::epsilon(); }
		//if (gameObject.boundingBox.back < bsp->boundingBox.back) { bsp->boundingBox.back = instance->boundingBox.back - std::numeric_limits<float>::epsilon(); }
		//if (gameObject.boundingBox.front > bsp->boundingBox.front) { bsp->boundingBox.front = instance->boundingBox.front + std::numeric_limits<float>::epsilon(); }
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
			childGameObjects.get()->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		BSP_Collect_Children_Specific(children.child1, childGameObjects);

		//do the same for child2 as above
		for (auto& i : children.child2->objects)
		{
			childGameObjects.get()->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		BSP_Collect_Children_Specific(children.child2, childGameObjects);
	}
}

void BinaryPartitionTree::BSP_Collect_Children_Specific(std::shared_ptr<BinaryPartitionTree> bsp, std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> childGameObjects)
{
	if (bsp->children.child1 != nullptr)
	{
		//while child1 exits, loop through the child and get all the instances
		//and add them to the childInstances vector
		for (auto& i : bsp->children.child1->objects)
		{
			childGameObjects.get()->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		BSP_Collect_Children_Specific(bsp->children.child1, childGameObjects);

		//do the same for child2 as above
		for (auto& i : bsp->children.child2->objects)
		{
			childGameObjects.get()->push_back(i.lock());
		}
		//recursively call the function to traves the tree of child bsps,
		//and collect their children, function continues past recursive call
		BSP_Collect_Children_Specific(bsp->children.child2, childGameObjects);
	}
}

void BinaryPartitionTree::BSP_Generate_Children()
{
	float splitLoc = meanContentsValue / objects.size();

	std::shared_ptr<BinaryPartitionTree> child1 = std::make_shared<BinaryPartitionTree>();
	child1.get()->Initialize(maxSubdivisions, minToSubdivide);
	child1.get()->parent = std::make_shared<BinaryPartitionTree>(*this);
	child1.get()->subdivisionLevel = subdivisionLevel + 1;

	//FIX
	//child1->boundingBox.left = bsp->boundingBox.left;
	//child1->boundingBox.right = bsp->boundingBox.right;
	//child1->boundingBox.front = bsp->boundingBox.front;
	//child1->boundingBox.back = bsp->boundingBox.back;
	//child1->boundingBox.top = bsp->boundingBox.top;
	//child1->boundingBox.bottom = bsp->boundingBox.bottom;

	std::shared_ptr<BinaryPartitionTree> child2 = std::make_shared<BinaryPartitionTree>();
	child2.get()->Initialize(maxSubdivisions, minToSubdivide);
	child2.get()->parent = std::make_shared<BinaryPartitionTree>(*this);
	child2.get()->subdivisionLevel = subdivisionLevel + 1;

	//FIX
	//child2->boundingBox.left = bsp->boundingBox.left;
	//child2->boundingBox.right = bsp->boundingBox.right;
	//child2->boundingBox.front = bsp->boundingBox.front;
	//child2->boundingBox.back = bsp->boundingBox.back;
	//child2->boundingBox.top = bsp->boundingBox.top;
	//child2->boundingBox.bottom = bsp->boundingBox.bottom;

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->FrontBack->TopBottom
	if (splitType == BSPSplitDirection::X)
	{
		child1.get()->splitType = BSPSplitDirection::Y;
		//child1->boundingBox.right = splitLoc;

		child2.get()->splitType = BSPSplitDirection::Y;
		//child2->boundingBox.left = splitLoc;
	}
	else if (splitType == BSPSplitDirection::Y)
	{
		child1.get()->splitType = BSPSplitDirection::Z;
		//child1->boundingBox.top = splitLoc;

		child2.get()->splitType = BSPSplitDirection::Z;
		//child2->boundingBox.bottom = splitLoc;
	}
	else
	{
		child1.get()->splitType = BSPSplitDirection::X;
		//child1->boundingBox.front = splitLoc;

		child2.get()->splitType = BSPSplitDirection::X;
		//child2->boundingBox.back = splitLoc;
	}

	children.child1 = child1;
	children.child2 = child2;
}

void BinaryPartitionTree::BSP_Generate_Children_Specific(std::shared_ptr<BinaryPartitionTree> bsp)
{
	float splitLoc = bsp.get()->meanContentsValue / bsp.get()->objects.size();

	std::shared_ptr<BinaryPartitionTree> child1 = std::make_shared<BinaryPartitionTree>();
	child1.get()->Initialize(bsp.get()->maxSubdivisions, bsp.get()->minToSubdivide);
	child1.get()->parent = std::make_shared<BinaryPartitionTree>(*this);
	child1.get()->subdivisionLevel = bsp.get()->subdivisionLevel + 1;

	//FIX
	//child1->boundingBox.left = bsp->boundingBox.left;
	//child1->boundingBox.right = bsp->boundingBox.right;
	//child1->boundingBox.front = bsp->boundingBox.front;
	//child1->boundingBox.back = bsp->boundingBox.back;
	//child1->boundingBox.top = bsp->boundingBox.top;
	//child1->boundingBox.bottom = bsp->boundingBox.bottom;

	std::shared_ptr<BinaryPartitionTree> child2 = std::make_shared<BinaryPartitionTree>();
	child2.get()->Initialize(bsp.get()->maxSubdivisions, bsp.get()->minToSubdivide);
	child2.get()->parent = std::make_shared<BinaryPartitionTree>(*this);
	child2.get()->subdivisionLevel = bsp.get()->subdivisionLevel + 1;

	//FIX
	//child2->boundingBox.left = bsp->boundingBox.left;
	//child2->boundingBox.right = bsp->boundingBox.right;
	//child2->boundingBox.front = bsp->boundingBox.front;
	//child2->boundingBox.back = bsp->boundingBox.back;
	//child2->boundingBox.top = bsp->boundingBox.top;
	//child2->boundingBox.bottom = bsp->boundingBox.bottom;

	//depending on the split direction, set the children bsp's values 
	//to the split location and iterate to the next enum value of split
	//direction, the order is LeftRight->FrontBack->TopBottom
	if (bsp.get()->splitType == BSPSplitDirection::X)
	{
		child1.get()->splitType = BSPSplitDirection::Y;
		//child1->boundingBox.right = splitLoc;

		child2.get()->splitType = BSPSplitDirection::Y;
		//child2->boundingBox.left = splitLoc;
	}
	else if (bsp.get()->splitType == BSPSplitDirection::Y)
	{
		child1.get()->splitType = BSPSplitDirection::Z;
		//child1->boundingBox.top = splitLoc;

		child2.get()->splitType = BSPSplitDirection::Z;
		//child2->boundingBox.bottom = splitLoc;
	}
	else
	{
		child1.get()->splitType = BSPSplitDirection::X;
		//child1->boundingBox.front = splitLoc;

		child2.get()->splitType = BSPSplitDirection::X;
		//child2->boundingBox.back = splitLoc;
	}

	bsp.get()->children.child1 = child1;
	bsp.get()->children.child2 = child2;
}

void BinaryPartitionTree::BSP_Clear_Node(bool root)
{
	if (initialized) return;

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
	if (bsp->initialized) return;

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
	//WIP !!!!!!!!!!!!!!!!!!!!!!! WIP

	//Remove Sizing
	//bounds.left = std::numeric_limits<float>::max();
	//bounds.right = std::numeric_limits<float>::min();
	//bounds.back = std::numeric_limits<float>::max();
	//bounds.front = std::numeric_limits<float>::min();
	//bounds.bottom = std::numeric_limits<float>::max();
	//bounds.top = std::numeric_limits<float>::min();
}

void BinaryPartitionTree::BSP_Place_Into(std::shared_ptr<GameObject> gameObject)
{
}
