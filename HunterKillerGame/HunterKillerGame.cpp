#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKillerBots/BaseBot.h"
#include "../HunterKillerBots/QuickRandomBot.h"
#include "../HunterKillerBots/RandomBot.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

void Init();
void Render(HunterKillerState*);
// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 792;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

SpriteRenderer* pRenderer;
int SPRITE_SIZE = 24;
std::vector<int>* foundation = new std::vector<int>();

int main()
{
	#pragma region Window setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hunter Killer", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// OpenGL configuration
	// --------------------
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	#pragma endregion

	auto* pActions = new std::vector<HunterKillerAction*>();
	auto* pActionResults = new std::vector<std::string>();
	auto* pFactory = new HunterKillerStateFactory();
	auto* pPlayer1Name = new std::string("A");
	auto* pPlayer2Name = new std::string("B");
    const auto* pPlayerNames = new std::vector{ pPlayer1Name, pPlayer2Name };

	// initialize game
	// ---------------
	Init();
	HunterKillerState* pState = pFactory->GenerateInitialState(*pPlayerNames);

	auto* bot = new RandomBot();

	// deltaTime variables
	// -------------------
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool finishedGame;
	do {
		// calculate delta time
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

        HunterKillerState* pStateCopy = pState->Copy();
		pStateCopy->Prepare(pState->GetActivePlayerID());

		HunterKillerAction* pAction = bot->Handle(*pStateCopy);
		pActions->push_back(pAction);

		Result* pResult = HunterKillerRules::Handle(*pState, *pAction);

		if (!pResult->Information->empty())
			pActionResults->push_back(std::string(*pResult->Information));

		std::cout << pState->GetMap().ToString() << std::endl;

		//std::this_thread::sleep_for(std::chrono::seconds(1));
		finishedGame = pResult->FinishedGame;

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		Render(pState);

		glfwSwapBuffers(window);

		delete pStateCopy; pStateCopy = nullptr;
		delete pResult; pResult = nullptr;
	} while (!finishedGame && !glfwWindowShouldClose(window));

	delete pRenderer; pRenderer = nullptr;
	glfwTerminate();
	return 0;
}

void Init()
{
	// load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    pRenderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
	#pragma region Units
    ResourceManager::LoadTexture("textures/infected_p1_0.png", true, "infected_p1_0");
	ResourceManager::LoadTexture("textures/medic_p1_0.png", true, "medic_p1_0");
	ResourceManager::LoadTexture("textures/soldier_p1_0.png", true, "soldier_p1_0");
	#pragma endregion
	#pragma region Bases
	ResourceManager::LoadTexture("textures/base_p1_0.png", false, "base_p1_0");
	#pragma endregion
	#pragma region Doors
	ResourceManager::LoadTexture("textures/door_closed.png", false, "door_closed");
	ResourceManager::LoadTexture("textures/door_open.png", true, "door_open");
	#pragma endregion
	#pragma region Floors
	ResourceManager::LoadTexture("textures/floor_0.png", false, "floor_0");
	ResourceManager::LoadTexture("textures/floor_1.png", false, "floor_1");
	ResourceManager::LoadTexture("textures/floor_2.png", false, "floor_2");
	ResourceManager::LoadTexture("textures/floor_3.png", false, "floor_3");
	ResourceManager::LoadTexture("textures/floor_4.png", false, "floor_4");
	ResourceManager::LoadTexture("textures/floor_5.png", false, "floor_5");
	ResourceManager::LoadTexture("textures/floor_6.png", false, "floor_6");
	ResourceManager::LoadTexture("textures/floor_7.png", false, "floor_7");
	#pragma endregion
	#pragma region Space
	ResourceManager::LoadTexture("textures/space_3.png", false, "space_3");
	#pragma endregion
	#pragma region Walls
	ResourceManager::LoadTexture("textures/wall_single.png", false, "wall_single");
	#pragma endregion

	// Randomize floor tiles
	std::uniform_int_distribution<int> uniform_dist(0, 7);
	int tilesOnScreen = ((SCREEN_HEIGHT / SPRITE_SIZE) + 1) * ((SCREEN_WIDTH / SPRITE_SIZE) + 1);
	for (int i = 0; i < tilesOnScreen; i++)
	{
		foundation->push_back(uniform_dist(HunterKillerConstants::RNG));
	}
}

void Render(HunterKillerState* pState)
{
	auto& rMap = pState->GetMap();
	int mapRenderWidth = rMap.GetMapWidth() * SPRITE_SIZE;
	int mapRenderHeight = rMap.GetMapHeight() * SPRITE_SIZE;
	auto& rMapContent = rMap.GetMapContent();
	for (int i = 0; i < rMapContent.size(); i++)
	{
		auto* pMapFeature = static_cast<MapFeature*>(rMapContent[i].at(HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX));
		auto* pUnit = static_cast<Unit*>(rMapContent[i].at(HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX));
		auto& rMapLocation = rMap.ToLocation(i);
		int x = rMapLocation.GetX() * SPRITE_SIZE;
		int y = rMapLocation.GetY() * SPRITE_SIZE;

		switch (pMapFeature->GetType()) {
		case FLOOR:
			pRenderer->DrawSprite(ResourceManager::GetTexture("floor_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		case WALL:
			pRenderer->DrawSprite(ResourceManager::GetTexture("wall_single"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		case DOOR_CLOSED:
			pRenderer->DrawSprite(ResourceManager::GetTexture("door_closed"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		case DOOR_OPEN: // Open doors need a floor as background
			pRenderer->DrawSprite(ResourceManager::GetTexture("floor_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			pRenderer->DrawSprite(ResourceManager::GetTexture("door_open"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		case SPACE:
			pRenderer->DrawSprite(ResourceManager::GetTexture("space_3"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
			break;
		default:
			// We're dealing with a structure...
			auto* pStructure = static_cast<Structure*>(pMapFeature);
			switch (pStructure->GetStructureType()) {
			case STRUCTURE_BASE:

			case STRUCTURE_OBJECTIVE:

			case STRUCTURE_OUTPOST:

			case STRUCTURE_STRONGHOLD:
				pRenderer->DrawSprite(ResourceManager::GetTexture("base_p1_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			}
			break;
		}

		//TODO: add decals on floor/wall tiles

		if (pUnit) {
			switch (pUnit->GetType()) {
			case UNIT_INFECTED:
				pRenderer->DrawSprite(ResourceManager::GetTexture("infected_p1_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			case UNIT_MEDIC:
				pRenderer->DrawSprite(ResourceManager::GetTexture("medic_p1_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			case UNIT_SOLDIER:
				pRenderer->DrawSprite(ResourceManager::GetTexture("soldier_p1_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				break;
			}
		}
	}
	/*
	int foundationIndex = 0;
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			if (x % 24 == 0 && y % 24 == 0) {
				std::string floorTexture = std::format("floor_{0:d}", foundation->at(foundationIndex));
				pRenderer->DrawSprite(ResourceManager::GetTexture(floorTexture), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(24.0f, 24.0f), 0.0f, glm::vec3(1.0f, 1.0f, 1.0f));
				++foundationIndex;
			}
		}
	}
	*/
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// when a user presses the escape key, we set the WindowShouldClose property to true, closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key >= 0 && key < 1024)
	{
		/*if (action == GLFW_PRESS)
			Breakout.Keys[key] = true;
		else if (action == GLFW_RELEASE) {
			Breakout.Keys[key] = false;
			Breakout.KeysProcessed[key] = false;
		}*/
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}