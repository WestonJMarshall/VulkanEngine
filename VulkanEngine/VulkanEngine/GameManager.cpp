#include "pch.h"
#include "GameManager.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "OctTree.h"
#include "KDTree.h"
#include "QuadTree.h"
#include "RigidShape.h"
#include "Rectangle.h"
#include "Circle.h"

#define MshMngr MeshManager::GetInstance()
KD_tree* KDtree;
std::vector<glm::vec2> linePoints;
bool octTreeDespawned = false;
bool linesActive = false;



#pragma region Singleton

GameManager* GameManager::instance = nullptr;

GameManager* GameManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new GameManager();
	}

	return instance;
}

#pragma endregion

#pragma region Accessors

std::vector<std::shared_ptr<Light>> GameManager::GetLights()
{
	return lights;
}

std::shared_ptr<GameObject> GameManager::GetObjectByName(std::string name)
{
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetName().compare(name) == 0) {
			return gameObjects[i];
		}
	}

	std::cout << "Could not find object with name: " << name << std::endl;
	return nullptr;
}

#pragma endregion

#pragma region Game Loop

void GameManager::Init()
{
	Camera::GetMainCamera()->SetPerspective(false);
	PhysicsManager::GetInstance()->SetGravityDirection(glm::vec3(0.0f, -1.0f, 0.0f));
	cameraSpeed = 5.0f;

	srand(time(NULL));

	//Setup Lights
	lights.push_back(std::make_shared<Light>(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.9f, 0.1f, 0.1f), 23.0f, 1.2f));

	//for (int i = 0; i < 75; i++) {

		//float randX = (float)rand() / (float)RAND_MAX;
		//float randRange = Range - -Range;
		//float finalRandX = (randX * randRange) + -Range;
		//float randY = (float)rand() / (float)RAND_MAX;
		//float finalRandY = (randY * randRange) + -Range;
		//float randZ = (float)rand() / (float)RAND_MAX;
		//float finalRandZ = (randZ * randRange) + -Range;

		//add a cube to the gameobject vector
		//gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));

		//set data, place position at random coords

		//if (i >= 50 && i <= 75) {
			//gameObjects[i]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(finalRandX, finalRandY, finalRandZ)));
		//}
		//else {
			//gameObjects[i]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(finalRandX, finalRandY, 0.0f)));
		//}

		//gameObjects[i]->GetTransform()->SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
		//gameObjects[i]->GetTransform()->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		//gameObjects[i]->SetName("Cube");
		//gameObjects[i]->Init();
	//}

	rigidShapes.push_back(std::make_shared<Rectangle>(Rectangle(glm::vec2(1.0, 2.0), 1.0f, 0.8f, 0.2f, 1.0f, 1.0f)));
	rigidShapes.push_back(std::make_shared<Rectangle>(Rectangle(glm::vec2(-1.0, 2.0), 1.0f, 0.8f, 0.2f, 1.0f, 1.0f)));
	rigidShapes.push_back(std::make_shared<Rectangle>(Rectangle(glm::vec2(0, -1.5), 0.0f, 0.8f, 0.2f, 8.0f, 0.5f)));
	rigidShapes.push_back(std::make_shared<Circle>(Circle(glm::vec2(0.0f, 2.0f), 1.0f, 0.8f, 0.2f, 0.5f)));


	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]));

	

	gameObjects[gameObjects.size() - 4]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(1.0f, 2.0f, 0)));
	gameObjects[gameObjects.size() - 4]->GetTransform()->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	gameObjects[gameObjects.size() - 4]->SetName("Cube1");

	gameObjects[gameObjects.size() - 3]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(-1.0f, 2.0f, 0)));
	gameObjects[gameObjects.size() - 3]->GetTransform()->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	gameObjects[gameObjects.size() - 3]->SetName("Cube2");

	gameObjects[gameObjects.size() - 2]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(0.0f, -1.5f, 0)));
	gameObjects[gameObjects.size() - 2]->GetTransform()->SetScale(glm::vec3(8.0f, 0.5f, 0.0f));
	gameObjects[gameObjects.size() - 2]->SetName("Floor");

	gameObjects[gameObjects.size() - 1]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(0.0f, 2.0f, 0)));
	gameObjects[gameObjects.size() - 1]->GetTransform()->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
	gameObjects[gameObjects.size() - 1]->SetName("Sphere1");

	gameObjects[gameObjects.size() - 4]->Spawn();
	gameObjects[gameObjects.size() - 3]->Spawn();
	gameObjects[gameObjects.size() - 2]->Spawn();
	gameObjects[gameObjects.size() - 1]->Spawn();

	/*
	for (int j = 0; j < rigidShapes.size(); j++)
	{
		for (int i = 0; i < rigidShapes[j]->getVertexes().size(); i++)
		{
			gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
			gameObjects[i + j * 4]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(rigidShapes[j]->getVertexes()[i].x, rigidShapes[j]->getVertexes()[i].y, 0)));
			gameObjects[i + j * 4]->GetTransform()->SetScale(glm::vec3(0.1f, 0.1f, 0.0f));
			gameObjects[i + j * 4]->Spawn();
		}
	}
	*/

	//Reset time so that it doesn't include initialization in totalTime
	Time::Reset();
}

void GameManager::Update()
{
	for (int i = gameObjects.size() - 1; i >= 0; i--)
	{
		if (gameObjects.size() != 2) {
			if (gameObjects[i]->GetTransform()->GetPosition().y < -10.0f) {
				//gameObjects[i]->Despawn();
				//rigidShapes.erase(rigidShapes.begin() + i);
				//gameObjects.erase(gameObjects.begin() + i);
			}
		}
	}

	if (InputManager::GetInstance()->GetKeyPressed(Controls::RightClick)) {
		float x = InputManager::GetInstance()->GetMousePosition().x;
		float y = InputManager::GetInstance()->GetMousePosition().y;

		//inputs
		float rangeXStartMouse = 0;
		float rangeXEndMouse = 800;
		float rangeYStartMouse = 0;
		float rangeYEndMouse = 600;

		//outputs
		float rangeXStartFloat = -7.0f;
		float rangeXEndFloat = 7.0f;
		float rangeYStartFloat = -5.0f;
		float rangeYEndFloat = 5.0f;

		float newX = (rangeXStartFloat + ((rangeXEndFloat - rangeXStartFloat) / (rangeXEndMouse - rangeXStartMouse)) * (x - rangeXStartMouse));
		float newY = -(rangeYStartFloat + ((rangeYEndFloat - rangeYStartFloat) / (rangeYEndMouse - rangeYStartMouse)) * (y - rangeYStartMouse));

		//std::cout << "Xpos mod: " << x << std::endl;
		//std::cout << "YPos mod: " << y << std::endl;


		//add a cube to the gameobject vector
		rigidShapes.push_back(std::make_shared<Rectangle>(Rectangle(glm::vec2(newX, newY), 1.0f, 0.8f, 0.2f, 0.5f, 0.5f)));
		gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));

		int lastIndex = gameObjects.size() - 1;
		int lastIndexR = rigidShapes.size() - 1;
	
		gameObjects[lastIndex]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(newX, newY, 0)));
		gameObjects[lastIndex]->GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		gameObjects[lastIndex]->SetName("Cube");
		 /*
		for (int i = 0; i < rigidShapes[lastIndexR]->getVertexes().size(); i++)
		{
			gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
			gameObjects[i + lastIndexR * 4]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(rigidShapes[lastIndexR]->getVertexes()[i].x, rigidShapes[lastIndexR]->getVertexes()[i].y, 0)));
			gameObjects[i + lastIndexR * 4]->GetTransform()->SetScale(glm::vec3(0.1f, 0.1f, 0.0f));
			gameObjects[i + lastIndexR * 4]->Spawn();
		}
		*/
		gameObjects[lastIndex]->Spawn();
	}

	if (InputManager::GetInstance()->GetKeyPressed(Controls::LeftClick)) {
		gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
		rigidShapes.push_back(std::make_shared<Rectangle>(Rectangle(glm::vec2(-10.0f, 1.0f), 1.0f, 0.8f, 0.2f, 0.5f, 0.5f)));

		int lastIndex = gameObjects.size() - 1;
		gameObjects[lastIndex]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(-10.0f, 1.0f, 0)));
		gameObjects[lastIndex]->GetTransform()->SetScale(glm::vec3(0.5f, 0.5f, 0.0f));
		gameObjects[lastIndex]->SetName("Cube");

		gameObjects[lastIndex]->Spawn();

		float y = InputManager::GetInstance()->GetMousePosition().y;

		float rangeYStartMouse = 0;
		float rangeYEndMouse = 600;
		float rangeYStartFloat = -5.0f;
		float rangeYEndFloat = 5.0f;

		float yAngle = -(rangeYStartFloat + ((rangeYEndFloat - rangeYStartFloat) / (rangeYEndMouse - rangeYStartMouse)) * (y - rangeYStartMouse));

		rigidShapes[lastIndex]->setVelocity(glm::vec2(15.0f, yAngle));
	}
	//Move Camera
	glm::vec3 moveDirection = glm::vec3(0.0f, 0.0f, 0.0f);

	if (InputManager::GetInstance()->GetKey(Controls::Up)) {
		Camera::GetMainCamera()->SetOrthographicSize(Camera::GetMainCamera()->GetOrthographicSize() * 1.001f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Down)) {
		Camera::GetMainCamera()->SetOrthographicSize(Camera::GetMainCamera()->GetOrthographicSize() * 0.999f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Back)) {
		//moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Forward)) {
		//moveDirection += glm::vec3(0.0f, -1.0f, 0.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Left)) {
		//moveDirection += glm::vec3(1.0f, 0.0f, 0.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Right)) {
		//moveDirection += glm::vec3(-1.0f, 0.0f, 0.0f);
	}

	if (moveDirection.x != 0 || moveDirection.y != 0 || moveDirection.z != 0) {
		moveDirection = glm::normalize(moveDirection);
	}

	Camera::GetMainCamera()->GetTransform()->Translate(moveDirection * cameraSpeed * Time::GetDeltaTime(), true);

	if (InputManager::GetInstance()->GetKey(Controls::Left)) {
	//	//gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(300.0f, glm::vec3(0, 1, 0)), false);
		gameObjects[1]->GetPhysicsObject()->ApplyForce(glm::vec3(-0.8f, 0.0f, 0.0f), false);
	}
	if (InputManager::GetInstance()->GetKeyPressed(Controls::Right)) {
	//	//gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(-300.0f, glm::vec3(0, 1, 0)), false);
		gameObjects[1]->GetPhysicsObject()->ApplyForce(glm::vec3(500.8f, 0.0f, 0.0f), false);
	}

	if (InputManager::GetInstance()->GetKeyPressed(Controls::Jump)) {
		gameObjects[1]->GetPhysicsObject()->ApplyForce(glm::vec3(0.0f, 1000.0f, 0.0f));
	}

	//Update Game Objects
	//for (size_t i = 0; i < gameObjects.size(); i++) {
		//gameObjects[i]->Update();
	//}

	for (int i = 0; i < rigidShapes.size(); i++)
	{
		rigidShapes[i]->update();
		gameObjects[i]->GetTransform()->SetPosition(glm::vec3(rigidShapes[i]->getCenter().x, rigidShapes[i]->getCenter().y, 0));
		gameObjects[i]->GetTransform()->SetOrientation(glm::vec3(0, 0, rigidShapes[i]->getRotation()));
		//std::cout << rigidShapes[i]->getCenter().x << ", " << rigidShapes[i]->getCenter().y << std::endl;
	}
	/*
	for (int j = 0; j < rigidShapes.size(); j++)
	{
		for (int i = 0; i < rigidShapes[j]->getVertexes().size(); i++)
		{
			gameObjects[i + j * 4]->GetTransform()->SetPosition(glm::vec3(rigidShapes[j]->getVertexes()[i].x, rigidShapes[j]->getVertexes()[i].y, 0));
		}
	}
	*/
}

void GameManager::UpdateShapes() 
{
	
}

void GameManager::CreateKDTree()
{
	if (octTreeDespawned == false) {
		OctTreeManager::despawnShapes();
	}
	octTreeDespawned = true;
	if (linesActive == true) {
		DebugManager::GetInstance()->RemoveAllShapes();
	}
	linesActive = true;

	for (int i = 0; i < KDtree->lineVectors.size(); i += 2)
	{
		if ((i + 1) < KDtree->lineVectors.size())
		{
			DebugManager::GetInstance()->DrawLine(KDtree->lineVectors[i], KDtree->lineVectors[i + 1], glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);
		}

	}

	for (int i = 0; i < entitycount; i++) {
		gameObjects[i]->Spawn();
	}
	for (int i = entitycount; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetActive() == true) {
			gameObjects[i]->Despawn();
		}
	}

}

void GameManager::CreateOctTree()
{
	OctTreeManager::spawnShapes();
	octTreeDespawned = false;

	if (linesActive == true) {
		DebugManager::GetInstance()->RemoveAllShapes();
	}
	linesActive = false;
	for (int i = 0; i < entitycount * 2; i++) {
		if (gameObjects[i]->GetActive() == true) {
			gameObjects[i]->Despawn();
		}

	}
	for (int i = entitycount * 2; i < gameObjects.size(); i++) {
		gameObjects[i]->Spawn();
	}

}


void GameManager::CreateQuadTree()
{
	if (octTreeDespawned == false) {
		OctTreeManager::despawnShapes();
	}
	octTreeDespawned = true;
	if (linesActive == true) {
		DebugManager::GetInstance()->RemoveAllShapes();
	}
	linesActive = true;

	//std::cout << "size of vector " << quadTree->Point.size() << std::endl;
	for (int i = 0; i < linePoints.size(); i += 2)
	{
		DebugManager::GetInstance()->DrawLine(glm::vec3(linePoints[i].x, linePoints[i].y, 0.0f), glm::vec3(linePoints[i + 1].x, linePoints[i + 1].y, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), -1.0f);
	}
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetActive() == true) {
			gameObjects[i]->Despawn();
		}
	}
	for (int i = entitycount; i < entitycount * 2; i++) {
		gameObjects[i]->Spawn();
	}
	for (int i = entitycount * 2; i < gameObjects.size(); i++) {
		if (gameObjects[i]->GetActive() == true) {
			gameObjects[i]->Despawn();
		}
	}
}

#pragma endregion