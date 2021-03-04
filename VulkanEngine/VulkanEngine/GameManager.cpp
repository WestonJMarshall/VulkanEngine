#include "pch.h"
#include "GameManager.h"

#include "DebugManager.h"
#include "EntityManager.h"
#include "InputManager.h"
#include "Camera.h"
#include "OctTree.h"
#include "KDTree.h"
#include "QuadTree.h"
#include "DrawKDTree.h"

#define MshMngr MeshManager::GetInstance()
QuadTree* quadTree;
std::vector<glm::vec2> linePoints;
int entitycount = 25;



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
    lights.push_back(std::make_shared<Light>(glm::vec3(1.5f, 1.1f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 5.0f));
    lights.push_back(std::make_shared<Light>(glm::vec3(0.0f, 2.0f, -1.5f), glm::vec3(1.0f, 0.988f, 0.769f), 3.0f, 4.0f));

    //gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Model]));
   
    // setup skybox
	

	OctTreeManager::InitOctTree(-2.0f, 2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 4, 2);
	OctTreeManager::despawnShapes();
	KD_tree* tree;
	Points_Tree.clear();
	quadTree = new QuadTree();
	activeOctTree = true;
	for (int i = 0; i < 75; i++) { //indexes 50 to 74, count of 75

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
		gameObjects[i]->SetTransform(std::make_shared<Transform>(glm::vec3()));
		gameObjects[i]->GetTransform()->SetPosition(glm::vec3(finalRandX, finalRandY, finalRandZ));
		gameObjects[i]->GetTransform()->SetOrientation(glm::vec3(0.0f, 0.0f, 0.0f));
		gameObjects[i]->GetTransform()->SetScale(glm::vec3(0.05f, 0.05f, 0.05f));
		gameObjects[i]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[i]->GetTransform(), PhysicsLayers::Trigger, 1.0f, false, false));
		gameObjects[i]->SetName("Cube");
	}

	//make quadTree
	std::cout << "DRAWING QUADTREE" << std::endl;
	for (int i = 0; i < 25; i++)
	{
		point_body_x[i] = gameObjects[i]->GetTransform()->GetPosition().x;
		point_body_y[i] = gameObjects[i]->GetTransform()->GetPosition().y;
		//bind the body points to the tree points
		//makes a check for negative and positive coords
		//then adds them to the tree point array
		//if for some reason the point is beyound the range,
		//set position as zero
		if (point_body_x[i] >= -Range && point_body_x[i] <= 0.00000001f) {
			point_tree_x[i] = roundf(point_body_x[i] * 1000) / 1000;
		}
		else if (point_body_x[i] >= 0.000001f && point_body_x[i] <= Range) {
			point_tree_x[i] = roundf(point_body_x[i] * 1000) / 1000;
		}
		else {
			point_tree_x[i] = 0;
		}

		if (point_body_y[i] >= -Range && point_body_y[i] <= 0.00000001f) {
			point_tree_y[i] = roundf(point_body_y[i] * 1000) / 1000;
		}
		else if (point_body_y[i] >= 0.000001f && point_body_y[i] <= Range) {
			point_tree_y[i] = roundf(point_body_y[i] * 1000) / 1000;
		}
		else {
			point_tree_y[i] = 0;
		}
		
	}
	for (int a = 0; a < entitycount; a++)
	{
		float _x = point_tree_x[a];
		float _y = point_tree_y[a];

		domain.push_back(_x);
		range.push_back(_y);

		point_temp.xpos = _x;
		point_temp.ypos = _y;
		//point_temp.id = a; for testing

		Points_Tree.push_back(point_temp);

		Point_Coord.push_back(_Point_xy(_x, _y));

	}

	//create the KD tree

	tree = new KD_tree(Points_Tree);
	tree->printInfo();
	tree->printTree();

	//draw it using debug lines
	drawKDTree(*(tree->get_Root()));

	for (int i = 25; i < 50; i++)
	{
		quadTree->Insert(gameObjects[i]->GetTransform()->GetPosition(), linePoints, maxCount);
	}
	for (int i = 50; i < 75; i++)
	{
		OctTreeManager::AddShape(gameObjects[i]);
	}

	//make skybox last
	gameObjects.push_back(std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Skybox]));
	int lastIndex = gameObjects.size() - 1;
	gameObjects[lastIndex]->SetTransform(std::make_shared<Transform>(glm::vec3(0)));
	gameObjects[lastIndex]->SetPhysicsObject(std::make_shared<PhysicsObject>(gameObjects[lastIndex]->GetTransform(), PhysicsLayers::Trigger, 1.0f, false, false));
	gameObjects[lastIndex]->SetName("Skybox");
	gameObjects[lastIndex]->Spawn();

    
}

void GameManager::Update()
{
	OctTreeManager::UpdateOctTree(activeOctTree);
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
	
	
    // MshMngr->ClearRenderList();
    // MeshManager::GetInstance()->DrawWireCube(glm::vec3(1.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //Rotate Camera
    //  Toggle camera lock on right click
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
        moveDirection += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (InputManager::GetInstance()->GetKey(Controls::Right)) {
        moveDirection += glm::vec3(-1.0f, 0.0f, 0.0f);
    }

    if (moveDirection.x != 0 || moveDirection.y != 0 || moveDirection.z != 0) {
        moveDirection = glm::normalize(moveDirection);
    }

    Camera::GetMainCamera()->GetTransform()->Translate(moveDirection * cameraSpeed * Time::GetDeltaTime(), true);

    //Update Lights
    float scaledTime = Time::GetTotalTime() / 2.5f;
    lights[0]->position = glm::vec3(0.0f, 1.1f, 0.0f) + glm::vec3(cos(scaledTime), 0.0f, sin(scaledTime)) * 1.5f;
    
    //Update Game Objects
    for (size_t i = 0; i < gameObjects.size(); i++) {
        gameObjects[i]->Update();
    }

    if (InputManager::GetInstance()->GetKeyPressed(Controls::Jump)) {
        gameObjects[2]->GetPhysicsObject()->ApplyForce(glm::vec3(0.0f, 5000.0f, 0.0f));

        //Spawn Object Sample Code:
        /*
        std::shared_ptr<GameObject> newObject = std::make_shared<GameObject>(EntityManager::GetInstance()->GetMeshes()[MeshTypes::Sphere]);
        gameObjects.push_back(newObject);

        newObject->SetTransform(std::make_shared<Transform>(glm::vec3(0.0f, 2.5f, 0.0f)));
        newObject->SetPhysicsObject(std::make_shared<PhysicsObject>(newObject->GetTransform(), PhysicsLayers::Static, 1.0f, false, true));

        newObject->Spawn();
        */
    }
}

void GameManager::CreateKDTree()
{
	OctTreeManager::despawnShapes();
	for (int i = 0; i < gameObjects.size(); i++) {
	   gameObjects[i]->Despawn();
   }
	for (int i = entitycount; i < entitycount*2; i++) {
		gameObjects[i]->Spawn();
	}
	for (int i = entitycount*2; i < gameObjects.size(); i++) {
		gameObjects[i]->Despawn();
	}
}

void GameManager::CreateOctTree()
{
	OctTreeManager::spawnShapes();
	for (int i = entitycount*2; i < gameObjects.size(); i++) {
		gameObjects[i]->Spawn();
	}
	for (int i = 0; i < entitycount*2; i++) {
		gameObjects[i]->Despawn();
	}
}

void GameManager::CreateQuadTree()
{
	OctTreeManager::despawnShapes();
	for (int i = 0; i < entitycount; i++) {
		gameObjects[i]->Spawn();
	}
	for (int i = entitycount; i < gameObjects.size(); i++) {
		gameObjects[i]->Despawn();
	}
	
}

#pragma endregion