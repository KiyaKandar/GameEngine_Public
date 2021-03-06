#include "TableCreation.h"

#include "DataBase.h"
#include "Table.h"
#include "../Profiler/Profiler.h"
#include "../UISystem/Input Managment/Input/InputManager.h"
#include "../nclgl/Rendering/View/Window.h"
#include "../GameLogicFSM/FSMManager.h"
#include "../Physics/Engine/PhysicsEngine.h"
#include "../nclgl/Rendering/View/Camera.h"
#include "../nclgl/InputDevices/Gamepad.h"
#include "../nclgl/Meshes/OBJMesh.h"
#include "../nclgl/Meshes/Mesh.h"
#include "../nclgl/Rendering/Renderer.h"
#include "../Game/Structure/PhysicsObject.h"
#include "../Game/Structure/GameObject.h"
#include "../Game/Players/Player.h"
#include "../Threading/ThreadPool/ThreadPool.h"
#include "../Game/Audio/AudioManager.h"

#include <vector>
#include "../Utilities/ErrorLog.h"
#include "../Utilities/StringUtility.h"

const size_t MAX_MEM_PER_TYPE = 5000;
const size_t MAX_MEM_PLAYERS = 7000;
const size_t MAX_MEM_OBJMESHES = 9000;
const size_t MAX_MEM_PHYSOBJ = 15000;

const int PHYS_OBJ_BINS = 5;
const int OBJMESH_BINS = 5;
const int SINGLE_BIN = 1;

TableCreation::TableCreation(DataBase* database)
{
	this->database = database;
	playerIDCount = new int(1);

	tableAdditions.push_back(std::bind(&TableCreation::AddProfiler, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddInputManager, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddPhysicsEngine, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddFSMManager, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddCamera, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddGamepads, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddOBJMeshes, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddMeshes, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddPhysicsObjects, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddGameObjects, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddPlayers, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddThreadPool, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddWindow, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddRenderer, this));
	tableAdditions.push_back(std::bind(&TableCreation::AddAudioManager, this));
}

TableCreation::~TableCreation()
{
	delete playerIDCount;
}

void TableCreation::AddTablesToDatabase() const
{
	for each (auto additionFunction in tableAdditions)
	{
		additionFunction();
	}
}

void TableCreation::AddProfiler() const
{
	database->AddTable("GProfiler", new Table<Resource>(false, false, [db = database](std::string params)
	{
		Profiler* profiler = new Profiler(db);
		profiler->ReadParams(params);

		return profiler;
	}, TableConfiguration("GProfiler", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddInputManager() const
{
	database->AddTable("GInputManager", new Table<Resource>(false, false, [db = database](std::string params)
	{
		Window* window = static_cast<Window*>(db->GetTable("GWindow")->GetResources()->Find("Window"));

		InputManager* inputManager = new InputManager(new Playerbase(db), window);
		inputManager->ReadParams(params);

		return inputManager;
	}, TableConfiguration("GInputManager", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddPhysicsEngine() const
{
	database->AddTable("PhysicsEngine", new Table<Resource>(false, false, [db = database](std::string params)
	{
		Renderer* renderer = static_cast<Renderer*>(db->GetTable("GRenderer")->GetResources()->Find("Renderer"));

		PhysicsEngine* physics = new PhysicsEngine(renderer, db);
		physics->ReadParams(params);

		return physics;
	}, TableConfiguration("PhysicsEngine", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddFSMManager() const
{
	database->AddTable("GFSMManager", new Table<Resource>(false, false, [](std::string params)
	{
		FSMManager* fsmm = new FSMManager();
		fsmm->ReadParams(params);

		return fsmm;
	}, TableConfiguration("GFSMManager", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddCamera() const
{
	database->AddTable("GCamera", new Table<Resource>(false, false, [](std::string params)
	{
		Camera* camera = new Camera();
		camera->ReadParams(params);

		return camera;
	}, TableConfiguration("GCamera", sizeof(Camera), SINGLE_BIN)));
}

void TableCreation::AddGamepads() const
{
	database->AddTable("Gamepads", new Table<Resource>(false, true, [](std::string params)
	{
		Gamepad* gamepad = new Gamepad();
		gamepad->ReadParams(params);

		return gamepad;
	}, TableConfiguration("Gamepads", sizeof(Gamepad) * 6, SINGLE_BIN)));
}

void TableCreation::AddOBJMeshes() const
{
	database->AddTable("OBJMeshes", new Table<Resource>(false, true, [](std::string params)
	{
		OBJMesh* objm = new OBJMesh(params);
		objm->Read(params);

		return objm;
	}, TableConfiguration("OBJMeshes", MAX_MEM_OBJMESHES, OBJMESH_BINS)));
}

void TableCreation::AddMeshes() const
{
	database->AddTable("Meshes", new Table<Resource>(false, true, [](std::string params)
	{
		Mesh* mesh = new Mesh();

		if (params == "quad")
		{
			mesh = mesh->GenerateQuad();
		}

		mesh->SetName(params);

		return mesh;
	}, TableConfiguration("Meshes", MAX_MEM_OBJMESHES, OBJMESH_BINS)));
}

void TableCreation::AddPhysicsObjects() const
{
	database->AddTable("PhysicsObjects", new Table<Resource>(false, true, [](std::string params)
	{
		PhysicsObject* physicsObject = new PhysicsObject();
		physicsObject->SetName(params);

		return physicsObject;
	}, TableConfiguration("PhysicsObjects", MAX_MEM_PHYSOBJ, PHYS_OBJ_BINS)));
}

void TableCreation::AddGameObjects() const
{
	database->AddTable("GameObjects", new Table<Resource>(false, true, [](std::string params)
	{
		GameObject* gameObject = new GameObject();
		gameObject->SetName(params);

		return gameObject;
	}, TableConfiguration("GameObjects", size_t(MAX_MEM_PHYSOBJ / 4), 2)));
}

void TableCreation::AddPlayers() const
{
	database->AddTable("Players", new Table<Resource>(false, true, [db = database, idCount = playerIDCount](std::string params)
	{
		Player* player = new Player(db, *idCount);
		player->SetName(params);

		*idCount = *idCount + 1;

		return player;
	}, TableConfiguration("Players", MAX_MEM_PLAYERS, SINGLE_BIN)));
}

void TableCreation::AddThreadPool() const
{
	database->AddTable("GThreadPool", new Table<Resource>(false, false, [](std::string params)
	{
		vector<string> tokens = StringUtility::Tokenise(params);
		string name = tokens.at(0);

		ThreadPool* threadPool = new ThreadPool();
		threadPool->SetName(name);

		return threadPool;
	}, TableConfiguration("GThreadPool", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddWindow() const
{
	database->AddTable("GWindow", new Table<Resource>(false, false, [db = database](std::string params)
	{
		vector<string> tokens = StringUtility::Tokenise(params);

		string name = tokens.at(0);
		int resX = atoi(tokens.at(1).c_str());
		int resY = atoi(tokens.at(2).c_str());
		bool fullscreen = atoi(tokens.at(3).c_str()) != 0;

		Window* window = new Window(db, name, resX, resY, fullscreen);
		window->SetName(name);

		return window;
	}, TableConfiguration("GWindow", sizeof(Window), SINGLE_BIN)));
}

void TableCreation::AddRenderer() const
{
	database->AddTable("GRenderer", new Table<Resource>(false, false, [db = database](std::string params)
	{
		Window* window = static_cast<Window*>(db->GetTable("GWindow")->GetResources()->Find("Window"));

		vector<string> tokens = StringUtility::Tokenise(params);

		string name = tokens.at(0);

		Renderer* renderer = new Renderer(db, *window);
		renderer->SetName(name);

		return renderer;
	}, TableConfiguration("GRenderer", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}

void TableCreation::AddAudioManager() const
{
	database->AddTable("GAudioManager", new Table<Resource>(false, false, [](std::string params)
	{
		AudioManager::GetInstance()->SetName(params);

		return AudioManager::GetInstance();
	}, TableConfiguration("GAudioManager", MAX_MEM_PER_TYPE, SINGLE_BIN)));
}
