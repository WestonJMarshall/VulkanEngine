#pragma once
#include "pch.h"

class PartitionTree
{
public:
	virtual int Initialize(int maxSubdivisions, int minToSubdivide) = 0;
	virtual int Fill(std::shared_ptr<GameObject> gameObject) = 0;
	virtual int Generate() = 0;
	virtual int Insert(std::shared_ptr<GameObject> gameObject) = 0;
	//virtual int Remove(std::shared_ptr<GameObject> gameObject) = 0;

	virtual int Draw() = 0;

	std::vector<std::weak_ptr<GameObject>> GetObjects()
	{
		return objects;
	}

protected:
	std::vector<std::weak_ptr<GameObject>> objects;
};

