#pragma once
#include "pch.h"

#include "GameObject.h"
#include "Octant.h";
#include "RigidShape.h"
#include "Rectangle.h"

class GameManager
{
private:
	static GameManager* instance;
	int entitycount = 25;
	
	std::vector<std::shared_ptr<GameObject>> octObjects;
	std::vector<std::shared_ptr<Light>> lights;
	

	std::shared_ptr<Octant> octree = nullptr;
	// Octant* octree = nullptr;

	float cameraSpeed = 2.5f;
	bool lockCamera = true;
public:
#pragma region Singleton
	int maxCount = 5;
	float Range = 2.0f;
	bool activeOctTree = false;
	std::vector<std::shared_ptr<RigidShape>> rigidShapes;
	std::vector<std::shared_ptr<GameObject>> gameObjects;
	static GameManager* GetInstance();

#pragma endregion

#pragma region Accessors

	std::vector<std::shared_ptr<Light>> GetLights();

	/// <summary>
	/// Finds a gameobject with the specified name
	/// </summary>
	/// <param name="name">The name of the object to find</param>
	/// <returns>The first object created with that name or null if there is no object found with the specified name</returns>
	std::shared_ptr<GameObject> GetObjectByName(std::string name);

#pragma endregion

#pragma region Game Loop

	/// <summary>
	/// Called before the first update, initializes the gameobjects
	/// </summary>
	void Init();

	/// <summary>
	/// Called once per frame, updates the gameobjects
	/// </summary>
	void Update();

	void UpdateShapes();

	void CreateKDTree();
	void CreateQuadTree();
	void CreateOctTree();

#pragma endregion
};