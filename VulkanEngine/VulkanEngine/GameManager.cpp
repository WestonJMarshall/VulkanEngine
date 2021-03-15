#include "pch.h"
#include "GameManager.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "OctTree.h"
#include "KDTree.h"
#include "QuadTree.h"

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
	srand(time(NULL));
	//Setup Lights
	lights.push_back(std::make_shared<Light>(glm::vec3(1.5f, 1.1f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 20.0f));
	lights.push_back(std::make_shared<Light>(glm::vec3(0.0f, 2.0f, -1.5f), glm::vec3(1.0f, 0.988f, 0.769f), 3.0f));

	for (int i = 0; i < 75; i++) {

		float randX = (float)rand() / (float)RAND_MAX;
		float randRange = Range - -Range;
		float finalRandX = (randX * randRange) + -Range;
		float randY = (float)rand() / (float)RAND_MAX;
		float finalRandY = (randY * randRange) + -Range;
		float randZ = (float)rand() / (float)RAND_MAX;
		float finalRandZ = (randZ * randRange) + -Range;

		//add a cube to the gameobject vector
		gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));

		//set data, place position at random coords

		if (i >= 50 && i <= 75) {
			gameObjects[i]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(finalRandX, finalRandY, finalRandZ)));
			//gameObjects[i]->GetTransform()->SetPosition(glm::vec3(finalRandX, finalRandY, finalRandZ));
		}
		else {
			gameObjects[i]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(finalRandX, finalRandY, 0.0f)));
		}

		gameObjects[i]->GetTransform()->SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
		gameObjects[i]->GetTransform()->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		gameObjects[i]->SetName("Cube");
		gameObjects[i]->Init();
		gameObjects[i]->Spawn();
	}
	for (int i = 0; i < 75; i++) {
		std::cout << "checking gameobj  " << gameObjects[i]->GetTransform()->GetPosition().y << std::endl;
	}

	// setup skybox
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Skybox]));
	int lastIndex = gameObjects.size() - 1;
	gameObjects[lastIndex]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(0)));
	gameObjects[lastIndex]->SetName("Skybox");
	gameObjects[lastIndex]->Init();
	gameObjects[lastIndex]->Spawn();

	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Cube]));

	gameObjects[gameObjects.size() - 3]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(0.0f, -1.0f, 0)));
	gameObjects[gameObjects.size() - 3]->GetTransform()->SetScale(glm::vec3(5.0f, 0.5f, 1.0f));
	gameObjects[gameObjects.size() - 3]->SetPhysicsObject(PhysicsLayers::Static, ColliderTypes::AABB, 1.0f, false);
	gameObjects[gameObjects.size() - 3]->SetName("Floor");

	gameObjects[gameObjects.size() - 2]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(-1.5f, 0.5f, 0)));
	gameObjects[gameObjects.size() - 2]->GetTransform()->SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
    gameObjects[gameObjects.size() - 2]->SetPhysicsObject(PhysicsLayers::Dynamic, ColliderTypes::AABB, 1.0f, true);
	gameObjects[gameObjects.size() - 2]->SetName("DCube1");

	gameObjects[gameObjects.size() - 1]->AddComponent<Transform>(std::make_shared<Transform>(glm::vec3(1.5f, 0.5f, 0)));
	gameObjects[gameObjects.size() - 1]->GetTransform()->SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
	gameObjects[gameObjects.size() - 1]->SetPhysicsObject(PhysicsLayers::Dynamic, ColliderTypes::AABB, 1.0f, true);
	gameObjects[gameObjects.size() - 1]->SetName("DCube2");

	gameObjects[gameObjects.size() - 3]->Init();
	gameObjects[gameObjects.size() - 2]->Init();
	gameObjects[gameObjects.size() - 1]->Init();
	gameObjects[gameObjects.size() - 3]->Spawn();
	gameObjects[gameObjects.size() - 2]->Spawn();
	gameObjects[gameObjects.size() - 1]->Spawn();

	//Reset time so that it doesn't include initialization in totalTime
	Time::Reset();
}

void GameManager::Update()
{
	// MshMngr->ClearRenderList();
	// MeshManager::GetInstance()->DrawWireCube(glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//Rotate Camera
	//  Toggle camera lock on right click
	OctTreeManager::UpdateOctTree(activeOctTree);
	if (activeOctTree) {
		OctTreeManager::despawnShapes();
	}
	activeOctTree = false;

	if (InputManager::GetInstance()->GetKeyPressed(Controls::KDTreeTog)) {
		CreateKDTree();
	}
	if (InputManager::GetInstance()->GetKeyPressed(Controls::QTreeTog)) {
		CreateQuadTree();
	}
	if (InputManager::GetInstance()->GetKeyPressed(Controls::OTreeTog)) {
		CreateOctTree();
	}

	if (InputManager::GetInstance()->GetKeyPressed(Controls::RightClick)) {
		lockCamera = !lockCamera;
	}
	if (InputManager::GetInstance()->GetKeyPressed(Controls::LeftClick)) {
		lockCamera = !lockCamera;
	}

	//  Rotate camera if not locked
	if (!lockCamera) {
		glm::vec2 deltaMouse = InputManager::GetInstance()->GetDeltaMouse();
		if (deltaMouse.x != 0 || deltaMouse.y != 0) {
			deltaMouse = glm::normalize(deltaMouse);
		}

		glm::quat orientation = Camera::GetMainCamera()->GetTransform()->GetOrientation();
		glm::vec3 rotation = orientation * glm::vec3(deltaMouse.y, 0.0f, 0.0f) + glm::vec3(0.0f, -deltaMouse.x, 0.0f);

		Camera::GetMainCamera()->GetTransform()->Rotate(rotation);
	}

	//Move Camera
	glm::vec3 moveDirection = glm::vec3(0.0f, 0.0f, 0.0f);

	if (InputManager::GetInstance()->GetKey(Controls::Forward)) {
		moveDirection += glm::vec3(0.0f, 0.0f, 1.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Back)) {
		moveDirection += glm::vec3(0.0f, 0.0f, -1.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Up)) {
		moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (InputManager::GetInstance()->GetKey(Controls::Down)) {
		moveDirection += glm::vec3(0.0f, -1.0f, 0.0f);
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

	//Update Lights
	float scaledTime = Time::GetTotalTime() / 2.5f;
	lights[0]->position = glm::vec3(0.0f, 1.1f, 0.0f) + glm::vec3(cos(scaledTime), 0.0f, sin(scaledTime)) * 1.5f;

	//gameObjects[0]->GetTransform()->Rotate(glm::vec3(0.0f, 10.0f, 0.0f) * Time::GetDeltaTime());

	if (InputManager::GetInstance()->GetKeyPressed(Controls::Left)) {
		gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(300.0f, glm::vec3(0, 1, 0)), false);
	}
	if (InputManager::GetInstance()->GetKeyPressed(Controls::Right)) {
		gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyTorque(glm::angleAxis(-300.0f, glm::vec3(0, 1, 0)), false);
		gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyForce(glm::vec3(500.0f, 0.0f, 0.0f));
	}

	//TODO: Remove when finished
	//Make sure axis are correct
	//gameObjects[2]->GetPhysicsObject()->GetVelocityAtPoint(gameObjects[2]->GetTransform()->GetPosition() + glm::vec3(1, 0, 0));

	if (InputManager::GetInstance()->GetKeyPressed(Controls::Jump)) {
		gameObjects[gameObjects.size() - 2]->GetPhysicsObject()->ApplyForce(glm::vec3(0.0f, 5000.0f, 0.0f));
	}

	//Update Game Objects
	for (size_t i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->Update();
	}
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