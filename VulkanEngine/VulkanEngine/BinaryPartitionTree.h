#pragma once
#include "PartitionTree.h"
#include "GameObject.h"
#include "PhysicsManager.h"

class BinaryPartitionTree : public PartitionTree
{
public:
	BinaryPartitionTree();

	int Initialize(int maxSubdivisions, int minToSubdivide);
	int Fill(std::shared_ptr<GameObject> gameObject);
	int Generate();
	int Draw();

	void SetMeansContentsValue(float value) { meanContentsValue = value; }
	float GetMeansContentsValue() { return meanContentsValue; }

	void SetBounds(glm::vec3 center, glm::vec3 extents);

	//BSP specific functions -------------------------------------------

	//Will appropriately size each BSP node if they
	//have more instances than needed to split 
	void BSP_Distribute_Down();

	//Collects all of the instances in the BSP's children and
	//puts them in childInstances
	void BSP_Collect_Children(std::shared_ptr<std::vector<std::shared_ptr<GameObject>>> childGameObjects);

private:
	//Helper classes
	enum class BSPSplitDirection
	{
		X,
		Y,
		Z
	};

	struct BSPChildren
	{
		std::shared_ptr<BinaryPartitionTree> child1;
		std::shared_ptr<BinaryPartitionTree> child2;
	};

	//BSP specific variables -------------------------------------------

	bool initialized = false;

	unsigned int index;
	std::shared_ptr<int> indexCount;

	BSPSplitDirection splitType;

	unsigned int subdivisionLevel;
	unsigned int maxSubdivisions;
	unsigned int minToSubdivide;

	std::shared_ptr<BinaryPartitionTree> parent;
	BSPChildren children;

	std::shared_ptr<PhysicsObject> bounds;

	float meanContentsValue; //determines where the BSP should split

	//Resize a root node with no children, nothing else
	void BSP_Basic_Resize(std::shared_ptr<GameObject> gameObject);

	//Self explainatory
	void BSP_Generate_Children();

	//Deletes all of the data in this node and all nodes below
	void BSP_Clear_Node(bool root = true);
	void BSP_Clear_Node_Specific(std::shared_ptr<BinaryPartitionTree> bsp, bool root = true);

	//Resets the bounds on the BSP
	void BSP_Clear_Resizing();

	//Call this to put an instance in an
	//already created BSP tree
	void BSP_Place_Into(std::shared_ptr<GameObject> gameObject);
};


