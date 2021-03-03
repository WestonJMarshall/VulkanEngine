#pragma once
#include "pch.h"
#include "KDTree.h"
#include "DebugManager.h"

struct bounds {
	float xMax = 2.0f;
	float yMax = 2.0f;
	float xMin = -2.0f;
	float yMin = -2.0f;
}base;

void drawRect()
{
	bounds bounds;
	DebugManager::GetInstance()->DrawLine(glm::vec3(bounds.xMin, bounds.yMax, 0.0f), glm::vec3(bounds.xMax, bounds.yMax, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

	DebugManager::GetInstance()->DrawLine(glm::vec3(bounds.xMax, bounds.yMax, 0.0f), glm::vec3(bounds.xMax, bounds.yMin, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

	DebugManager::GetInstance()->DrawLine(glm::vec3(bounds.xMax, bounds.yMin, 0.0f), glm::vec3(bounds.xMin, bounds.yMin, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

	DebugManager::GetInstance()->DrawLine(glm::vec3(bounds.xMin, bounds.yMin, 0.0f), glm::vec3(bounds.xMin, bounds.yMax, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);
}

void drawNode(KD_Node node, bounds space)
{
	float x = node.Get_point().xpos;
	float y = node.Get_point().ypos;

	bounds leftSpace = space;
	bounds rightSpace = space;

	//vertical
	if (node.Get_type() == 100) {
		DebugManager::GetInstance()->DrawLine(glm::vec3(x, space.yMax, 0.0f), glm::vec3(x, space.yMin, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

		leftSpace.xMax = x;
		rightSpace.xMin = x;
	}
	//horizontal
	else if (node.Get_type() == 101) {
		DebugManager::GetInstance()->DrawLine(glm::vec3(space.xMin, y, 0.0f), glm::vec3(space.xMax, y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);

		leftSpace.yMax = y;
		rightSpace.yMin = y;
	}

	if (!node.isleaf()) {
		if (node.Get_leftTree() != nullptr) {
			drawNode(*node.Get_leftTree(), leftSpace);
		}
		if (node.Get_rightTree() != nullptr) {
			drawNode(*node.Get_rightTree(), rightSpace);
		}
	}

}

void drawKDTree(KD_Node root) {
	drawRect();
	drawNode(root, base);
}
