#pragma once
#include "pch.h"

#include "GameObject.h"


class GameManager
{
private:
	static GameManager* instance;


	std::vector<std::shared_ptr<Light>> lights;

	float cameraSpeed = 2.5f;
	bool lockCamera = true;
public:
	
	int maxCount = 5;
	float Range = 2.0f;
	bool activeOctTree = false;
	std::vector<std::shared_ptr<GameObject>> gameObjects;

#pragma region Singleton

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

	void Init();
	void Update();
	void CreateKDTree();
	void CreateQuadTree();
	void CreateOctTree();
	

#pragma endregion
};