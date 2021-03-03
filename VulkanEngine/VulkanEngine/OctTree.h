#pragma once
#include "pch.h"
#include "GameObject.h"

class InteractiveShape;
class RenderShape;

struct Collider
{
	float width;
	float height;
	float depth;
	float x;
	float y;
	float z;
};

struct OctTreeNode
{
	std::vector<std::shared_ptr<GameObject>> shapes;
	std::shared_ptr<GameObject> outline;
	int children[8];
	int parent;

	bool active;
	bool hasChildren;
	unsigned int depth;
	float left;
	float right;
	float top;
	float bottom;
	float front;
	float back;

};

class OctTreeManager
{
public:


	static void InitOctTree(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode);

	static void UpdateOctTree();

	static void AddShape(std::shared_ptr<GameObject> shape);

	static void DumpData();

	static const std::vector<std::shared_ptr<GameObject>>& GetNearbyShapes(std::shared_ptr<GameObject> shape);

private:

	static void AddShape(std::shared_ptr<GameObject> shape, int startingNode);

	static int CheckShapeNodeCollide(std::shared_ptr<GameObject> shape, OctTreeNode* node);

	static void InitChildren(int nodeIndex);

	static OctTreeNode* InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back);

	static void ActivateChildren(OctTreeNode* parent);

	static void DeactivateNode(OctTreeNode* node);

	static void ResetTree();

	static void ActivateNode(OctTreeNode* node);

	static int GetDepthIndex(int depth);

	static std::vector<OctTreeNode*> _octTree;
	static std::vector<std::shared_ptr<GameObject>> _shapes;
	static unsigned int _maxDepth;
	static unsigned int _maxPerNode;
	static std::shared_ptr<GameObject> _outlineTemplate;
	static bool needsToUpdate;


};