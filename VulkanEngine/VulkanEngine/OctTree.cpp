#include "pch.h"
#include "OctTree.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "PhysicsManager.h"
#include "EntityManager.h"
#include "GameManager.h"
#include <stack>

std::vector<OctTreeNode*> OctTreeManager::_octTree = std::vector<OctTreeNode*>();
std::vector<std::shared_ptr<GameObject>> OctTreeManager::_shapes = std::vector<std::shared_ptr<GameObject>>();
unsigned int OctTreeManager::_maxDepth = 0;
unsigned int OctTreeManager::_maxPerNode = 0;
bool OctTreeManager::needsToUpdate = true;
//std::shared_ptr<GameObject> OctTreeManager::_outlineTemplate;

// When the oct-tree manager is initialized, it instantiates the entire possible tree to avoid having to do a bunch of 
// time wasting news and deletes during runtime.
void OctTreeManager::InitOctTree(float left, float right, float top, float bottom, float front, float back, unsigned int maxDepth, unsigned int maxPerNode)
{
	_maxPerNode = maxPerNode;
	_maxDepth = maxDepth;
	//_outlineTemplate = outlineTemplate;
	_octTree.resize(GetDepthIndex(maxDepth));
	_octTree[0] = InitNode(0, 0, 0, left, right, top, bottom, front, back);
	int maxI = GetDepthIndex(maxDepth - 1);
	for (int i = 0; i < maxI; i++)
	{
		InitChildren(i);
	}

}


// When updateing the tree, the manager goes throughand deactivates every node in the tree and then reactivates the root.
// It then goes through the entire array of interactive shapes and adds them back into the tree. 
void OctTreeManager::UpdateOctTree(bool update)
{
	if (update == true) {
		ResetTree();
		ActivateNode(_octTree[0]);
		unsigned int shapesSize = _shapes.size();
		for (unsigned int i = 0; i < shapesSize; i++) {
			AddShape(_shapes[i], 0);
		}
	}
}

void OctTreeManager::AddShape(std::shared_ptr<GameObject> shape) {
	_shapes.push_back(shape);
}

// When the program ends, the manager has to go through and delete all of the nodes of quad tree that it instantiated
// during the init function.
void OctTreeManager::DumpData()
{
	despawnShapes();
	_shapes.clear();
	_octTree.clear();
	
}

// Retrieves all the shapes that share a node with the shape passed in. It uses a method similar to when a shape is being
// added to the tree. When it gets to the lowest node that the argument shape collides with, it returns the array of shapes
// associated with that node
const std::vector<std::shared_ptr<GameObject>>& OctTreeManager::GetNearbyShapes(std::shared_ptr<GameObject> shape)
{
	unsigned int treeSize = _octTree.size();
	for (unsigned int i = 0; i < treeSize;) {
		OctTreeNode* currentNode = _octTree[i];
		int result = CheckShapeNodeCollide(shape, currentNode);

		//no collision
		if (result == 0) {
			++i;
			continue;
		}
		//partial collision
		if (result == 1) {
			if (currentNode->depth != 0) {
				return _octTree[currentNode->parent]->shapes;
			}
			else {
				result = 2;
			}
		}

		//full collision
		if (result == 2)
		{
			if (!currentNode->hasChildren) {
				return currentNode->shapes;
			}
			else {
				i = currentNode->children[0];
			}
		}
	}
}

// Adds the given shape to the oct-tree beginnng at the node index passed in. Shapes are added to the first node that with which they have a successful collision
// If they only have a partial collision, they are added to that node's parent. If a node is at the bottom of the activated tree, and it exceeds the max number
// of shapes, then each of it's shapes are added back into the tree, passing that node's index as the starting node and that node's children are activated.
void OctTreeManager::AddShape(std::shared_ptr<GameObject> shape, int startingNode)
{
	unsigned int treeSize = _octTree.size();
	for (unsigned int i = startingNode; i < treeSize;) {
		OctTreeNode* currentNode = _octTree[i];
		int result = CheckShapeNodeCollide(shape, currentNode);
		//no collision
		if (result == 0) {
			++i;
			continue;
		}
		//partial collision
		if (result == 1) {
			if (currentNode->depth != 0) {
				_octTree[currentNode->parent]->shapes.push_back(shape);
				break;
			}
			else {
				result = 2;
			}
		}
		//full collision
		if (result == 2) {
			if (currentNode->shapes.size() >= _maxPerNode && currentNode->depth < _maxDepth) {
				if (!currentNode->hasChildren) {
					ActivateChildren(currentNode);
					//Add all the shapes in the current node to the current node's children
					std::vector<std::shared_ptr<GameObject>> shapesTemp = currentNode->shapes;
					currentNode->shapes.clear();
					unsigned int size = shapesTemp.size();
					for (unsigned int j = 0; j < size; j++) {
						AddShape(shapesTemp[j], currentNode->children[0]);
					}

				}
				i = currentNode->children[0];
			}
			else {
				if (!currentNode->hasChildren) {
					currentNode->shapes.push_back(shape);
					break;
				}
				else {
					i = currentNode->children[0];
				}
			}
		}
	}
}

int OctTreeManager::CheckShapeNodeCollide(std::shared_ptr<GameObject> shape, OctTreeNode* node)
{
	// 0 - No Collision
	// 1 - partial collision
	// 2 - full collision
	int colStatus = 0;
	//Collider col = shape->GetPhysicsObject()->GetTransform();           //would the y scale be height?
	float dTop = node->top - (shape->GetTransform()->GetPosition().y + shape->GetTransform()->GetScale().y / 2.0f);
	float dBot = node->bottom - (shape->GetTransform()->GetPosition().y - shape->GetTransform()->GetScale().y / 2.0f);
	float dLeft = node->left - (shape->GetTransform()->GetPosition().x - shape->GetTransform()->GetScale().x / 2.0f);
	float dRight = node->right - (shape->GetTransform()->GetPosition().x + shape->GetTransform()->GetScale().x / 2.0f);
	float dFront = node->front - (shape->GetTransform()->GetPosition().z - shape->GetTransform()->GetScale().z / 2.0f);
	float dBack = node->back - (shape->GetTransform()->GetPosition().z + shape->GetTransform()->GetScale().z / 2.0f);
	float width = node->right - node->left;
	float height = node->top - node->bottom;
	float depth = node->front - node->back;
	colStatus += abs(dTop) < height && abs(dBot) < height && abs(dRight) < width && abs(dLeft) < width && abs(dFront) < depth && abs(dBack) < depth;
	colStatus += dTop > 0 && dBot < 0 && dRight > 0 && dLeft < 0 && dFront > 0 && dBack < 0;
	if (colStatus == 0) {
		//std::cout << "No collision detected." << std::endl;
	}
	if (colStatus == 1) {
		//std::cout << "Partial collision detected." << std::endl;
	}
	if (colStatus == 2) {
		//std::cout << "Full collision detected." << std::endl;
	}
	return colStatus;
}

void OctTreeManager::InitChildren(int nodeIndex)
{
	OctTreeNode* node = _octTree[nodeIndex];
	float midX = node->left + ((node->right - node->left) / 2.0f);
	float midY = node->bottom + ((node->top - node->bottom) / 2.0f);
	float midZ = node->back + ((node->front - node->back) / 2.0f);
	int childNum = node->children[0] - GetDepthIndex(node->depth);
	_octTree[node->children[0]] = InitNode(node->depth + 1, nodeIndex, childNum, node->left, midX, node->top, midY, node->front, midZ);
	_octTree[node->children[1]] = InitNode(node->depth + 1, nodeIndex, childNum + 1, midX, node->right, node->top, midY, node->front, midZ);
	_octTree[node->children[2]] = InitNode(node->depth + 1, nodeIndex, childNum + 2, node->left, midX, midY, node->bottom, node->front, midZ);
	_octTree[node->children[3]] = InitNode(node->depth + 1, nodeIndex, childNum + 3, midX, node->right, midY, node->bottom, node->front, midZ);
	_octTree[node->children[4]] = InitNode(node->depth + 1, nodeIndex, childNum + 4, node->left, midX, node->top, midY, midZ, node->back);
	_octTree[node->children[5]] = InitNode(node->depth + 1, nodeIndex, childNum + 5, midX, node->right, node->top, midY, midZ, node->back);
	_octTree[node->children[6]] = InitNode(node->depth + 1, nodeIndex, childNum + 6, node->left, midX, midY, node->bottom, midZ, node->back);
	_octTree[node->children[7]] = InitNode(node->depth + 1, nodeIndex, childNum + 7, midX, node->right, node->top, midY, node->front, midZ);
	

}

OctTreeNode* OctTreeManager::InitNode(int depth, int parentIndex, int childNum, float left, float right, float top, float bottom, float front, float back)
{
	OctTreeNode* node = new OctTreeNode();
	node->active = false;
	node->hasChildren = false;
	node->depth = depth;
	node->left = left;
	node->right = right;
	node->top = top;
	node->bottom = bottom;
	node->front = front;
	node->back = back;

	//make wireframe gameobject for now(?) def not doing rendershape
	std::shared_ptr<GameObject> outline = std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::WireCube]);
	//GameManager::GetInstance()->gameObjects.push_back(outline);
	node->outline = outline;



	int base = GetDepthIndex(depth) + childNum * 8;
	node->children[0] = base;
	node->children[1] = base + 1;
	node->children[2] = base + 2;
	node->children[3] = base + 3;
	node->children[4] = base + 4;
	node->children[5] = base + 5;
	node->children[6] = base + 6;
	node->children[7] = base + 7;
	node->parent = parentIndex;

	node->outline->SetTransform(std::make_shared<Transform>(glm::vec3()));
	node->outline->GetTransform()->SetPosition(glm::vec3((node->left + node->right) / 2.0f, (node->top + node->bottom) / 2.0f, (node->front + node->back) / 2.0f));
	node->outline->GetTransform()->SetScale(glm::vec3((node->right - node->left), (node->top - node->bottom), (node->front - node->back)));
	node->outline->SetPhysicsObject(std::make_shared<PhysicsObject>(node->outline->GetTransform(), PhysicsLayers::Trigger, 0.0f, false, false));
	//std::cout << outline->GetTransform()->GetPosition().x << std::endl;//std::cout << outline->GetTransform()->GetPosition().x << std::endl;

	return node;
}

void OctTreeManager::ActivateChildren(OctTreeNode* parent)
{
	parent->hasChildren = true;
	ActivateNode(_octTree[parent->children[0]]);
	ActivateNode(_octTree[parent->children[1]]);
	ActivateNode(_octTree[parent->children[2]]);
	ActivateNode(_octTree[parent->children[3]]);
	ActivateNode(_octTree[parent->children[4]]);
	ActivateNode(_octTree[parent->children[5]]);
	ActivateNode(_octTree[parent->children[6]]);
	ActivateNode(_octTree[parent->children[7]]);

}

void OctTreeManager::despawnShapes() {
	
	for (int i = _octTree.size() - 1; i >= 0; i--)
	{
		if (_octTree[i]->active) {
			_octTree[i]->outline->Despawn();
		}
	}
	
}

void OctTreeManager::spawnShapes() {

	for (int i = 0; i < _octTree.size(); i++)
	{
		if (_octTree[i]->active) {
			_octTree[i]->outline->Spawn();
		}
	}

}

void OctTreeManager::ResetTree()
{
	std::stack<int> stack;
	stack.push(0);

	while (!stack.empty()) {
		OctTreeNode* node = _octTree[stack.top()];
		stack.pop();

		if (node->active) {
			node->active = false;
			node->shapes.clear();
			node->outline->Despawn();

			if (node->hasChildren) {
				stack.push(node->children[0]);
				stack.push(node->children[1]);
				stack.push(node->children[2]);
				stack.push(node->children[3]);
				stack.push(node->children[4]);
				stack.push(node->children[5]);
				stack.push(node->children[6]);
				stack.push(node->children[7]);
			}
		}
	}
}

void OctTreeManager::ActivateNode(OctTreeNode* node)
{
	node->active = true;
	//gameObject activation? may not be neciscary
	node->hasChildren = false;
	//spawn upon activation?
	node->outline->Spawn();

}

int OctTreeManager::GetDepthIndex(int depth)
{
	float depthf = (float)depth;
	float retf = 0.0f;
	for (float i = 0.0f; i < depthf || i == depthf; i += 1.0f)
	{
		retf += powf(8.0f, i);
	}
	int ret = (int)retf;
	return ret;
}