#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/Wall.h"
#include "../HunterKillerBots/BaseBot.h"
#include "../HunterKillerBots/QuickRandomBot.h"
#include "../HunterKillerBots/RandomBot.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"

void InitRendering();
void Render(HunterKillerState*, HunterKillerAction*);
bool isWalled(std::vector<std::vector<MapFeature*>>&, int, int);
int determineWallMask(HunterKillerMap& rMap, MapLocation& rLocation);
int sample(double weight, int collectionSize);
// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

SpriteRenderer* pRenderer;
TextRenderer* pText;
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 600;
unsigned int MAP_WIDTH = 0;
unsigned int MAP_HEIGHT = 0;
const int SPRITE_SIZE = 24;
const float TEXT_OFFSET = 8.0f;
const int UP_MASK = 1, RIGHT_MASK = 2, DOWN_MASK = 4, LEFT_MASK = 8;
const glm::vec3 COLOR_WHITE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 COLOR_BLACK = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 COLOR_BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 COLOR_NAVY = glm::vec3(0.0f, 0.0f, 0.5f);
const glm::vec3 COLOR_CYAN = glm::vec3(0.0f, 1.0f, 1.0f);
const glm::vec3 COLOR_TEAL = glm::vec3(0.0f, 0.5f, 0.5f);
const glm::vec3 COLOR_GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 COLOR_YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
const glm::vec3 COLOR_RED = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 COLOR_PINK = glm::vec3(1.0f, 0.753f, 0.796f);
const glm::vec3 COLOR_MAGENTA = glm::vec3(1.0f, 0.0f, 1.0f);
const glm::vec3 COLOR_UI_TEXT = glm::vec3(0.145f, 0.588f, 0.745f);
std::vector<int>* pFloorVariations = new std::vector<int>();
std::vector<int>* pFloorDecorations = new std::vector<int>();
std::vector<int>* pSpaceVariations = new std::vector<int>();
std::vector<int>* pWallVerticalVariations = new std::vector<int>();
std::vector<int>* pWallHorizontalVariations = new std::vector<int>();

int main()
{
	// initialize game
	// ---------------
	auto* pActions = new std::vector<HunterKillerAction*>();
	auto* pActionResults = new std::vector<std::string>();
	auto* pFactory = new HunterKillerStateFactory();
	auto* pPlayer1Name = new std::string("A");
	auto* pPlayer2Name = new std::string("B");
    const auto* pPlayerNames = new std::vector{ pPlayer1Name, pPlayer2Name };
	HunterKillerState* pState = pFactory->GenerateInitialState(*pPlayerNames);
	MAP_WIDTH = pState->GetMap().GetMapWidth() * SPRITE_SIZE;
	MAP_HEIGHT = pState->GetMap().GetMapHeight() * SPRITE_SIZE;

	#pragma region Window setup
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	// Window Title
	// ------------
	std::string windowTitle = std::format("Hunter Killer | {0}", pState->GetMap().GetName());
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, windowTitle.c_str(), nullptr, nullptr);
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

	// Window Icon
	// -----------
	GLFWimage images[1]{};
	images[0].pixels = stbi_load("textures/units/infected_p1_0.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images); 
	stbi_image_free(images[0].pixels);

	#pragma endregion

	InitRendering();

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

		//std::cout << pState->GetMap().ToString() << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		finishedGame = pResult->FinishedGame;

		// render
		// ------
		glClearColor(0.824f, 0.733f, 0.545f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT);
		Render(pState, pAction);

		glfwSwapBuffers(window);

		delete pStateCopy; pStateCopy = nullptr;
		delete pResult; pResult = nullptr;
	} while (!finishedGame && !glfwWindowShouldClose(window));
	
	glfwTerminate();
	#pragma region Cleanup
	delete bot; bot = nullptr;
	delete pWallHorizontalVariations; pWallHorizontalVariations = nullptr;
	delete pWallVerticalVariations; pWallVerticalVariations = nullptr;
	delete pSpaceVariations; pSpaceVariations = nullptr;
	delete pFloorDecorations; pFloorDecorations = nullptr;
	delete pFloorVariations; pFloorVariations = nullptr;
	delete pText; pText = nullptr;
	delete pRenderer; pRenderer = nullptr;
	delete pPlayerNames; pPlayerNames = nullptr;
	delete pPlayer2Name; pPlayer2Name = nullptr;
	delete pPlayer1Name; pPlayer1Name = nullptr;
	delete pFactory; pFactory = nullptr;
	delete pActionResults; pActionResults = nullptr;
	delete pActions; pActions = nullptr;
	#pragma endregion

	return 0;
}

void InitRendering()
{
	// Load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // Set render-specific controls
    pRenderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	pText = new TextRenderer(SCREEN_WIDTH, SCREEN_HEIGHT);
    pText->Load("fonts/font.ttf", 24);
    // Load textures
	#pragma region Units
    ResourceManager::LoadTexture("textures/units/infected_p1_0.png", true, "infected_p1_0");
	ResourceManager::LoadTexture("textures/units/infected_p1_1.png", true, "infected_p1_1");
	ResourceManager::LoadTexture("textures/units/infected_p2_0.png", true, "infected_p2_0");
	ResourceManager::LoadTexture("textures/units/infected_p2_1.png", true, "infected_p2_1");
	ResourceManager::LoadTexture("textures/units/infected_p3_0.png", true, "infected_p3_0");
	ResourceManager::LoadTexture("textures/units/infected_p3_1.png", true, "infected_p3_1");
	ResourceManager::LoadTexture("textures/units/infected_p4_0.png", true, "infected_p4_0");
	ResourceManager::LoadTexture("textures/units/infected_p4_1.png", true, "infected_p4_1");
	ResourceManager::LoadTexture("textures/units/medic_p1_0.png", true, "medic_p1_0");
	ResourceManager::LoadTexture("textures/units/medic_p1_1.png", true, "medic_p1_1");
	ResourceManager::LoadTexture("textures/units/medic_p2_0.png", true, "medic_p2_0");
	ResourceManager::LoadTexture("textures/units/medic_p2_1.png", true, "medic_p2_1");
	ResourceManager::LoadTexture("textures/units/medic_p3_0.png", true, "medic_p3_0");
	ResourceManager::LoadTexture("textures/units/medic_p3_1.png", true, "medic_p3_1");
	ResourceManager::LoadTexture("textures/units/medic_p4_0.png", true, "medic_p4_0");
	ResourceManager::LoadTexture("textures/units/medic_p4_1.png", true, "medic_p4_1");
	ResourceManager::LoadTexture("textures/units/soldier_p1_0.png", true, "soldier_p1_0");
	ResourceManager::LoadTexture("textures/units/soldier_p1_1.png", true, "soldier_p1_1");
	ResourceManager::LoadTexture("textures/units/soldier_p2_0.png", true, "soldier_p2_0");
	ResourceManager::LoadTexture("textures/units/soldier_p2_1.png", true, "soldier_p2_1");
	ResourceManager::LoadTexture("textures/units/soldier_p3_0.png", true, "soldier_p3_0");
	ResourceManager::LoadTexture("textures/units/soldier_p3_1.png", true, "soldier_p3_1");
	ResourceManager::LoadTexture("textures/units/soldier_p4_0.png", true, "soldier_p4_0");
	ResourceManager::LoadTexture("textures/units/soldier_p4_1.png", true, "soldier_p4_1");
	#pragma endregion
	#pragma region Bases
	ResourceManager::LoadTexture("textures/structures/base_p1_0.png", false, "base_p1_0");
	ResourceManager::LoadTexture("textures/structures/base_p1_1.png", false, "base_p1_1");
	ResourceManager::LoadTexture("textures/structures/base_p1_2.png", true, "base_p1_2");
	ResourceManager::LoadTexture("textures/structures/base_p1_3.png", true, "base_p1_3");
	ResourceManager::LoadTexture("textures/structures/base_p2_0.png", false, "base_p2_0");
	ResourceManager::LoadTexture("textures/structures/base_p2_1.png", false, "base_p2_1");
	ResourceManager::LoadTexture("textures/structures/base_p2_2.png", true, "base_p2_2");
	ResourceManager::LoadTexture("textures/structures/base_p2_3.png", true, "base_p2_3");
	ResourceManager::LoadTexture("textures/structures/base_p3_0.png", false, "base_p3_0");
	ResourceManager::LoadTexture("textures/structures/base_p3_1.png", false, "base_p3_1");
	ResourceManager::LoadTexture("textures/structures/base_p3_2.png", true, "base_p3_2");
	ResourceManager::LoadTexture("textures/structures/base_p3_3.png", true, "base_p3_3");
	ResourceManager::LoadTexture("textures/structures/base_p4_0.png", false, "base_p4_0");
	ResourceManager::LoadTexture("textures/structures/base_p4_1.png", false, "base_p4_1");
	ResourceManager::LoadTexture("textures/structures/base_p4_2.png", true, "base_p4_2");
	ResourceManager::LoadTexture("textures/structures/base_p4_3.png", true, "base_p4_3");
	ResourceManager::LoadTexture("textures/structures/base_p5_1.png", false, "base_p5_1");
	ResourceManager::LoadTexture("textures/structures/base_p5_2.png", true, "base_p5_2");
	ResourceManager::LoadTexture("textures/structures/base_p5_3.png", true, "base_p5_3");
	#pragma endregion
	#pragma region Doors
	ResourceManager::LoadTexture("textures/map/door_closed.png", false, "door_closed");
	ResourceManager::LoadTexture("textures/map/door_open.png", true, "door_open");
	#pragma endregion
	#pragma region Floors
	ResourceManager::LoadTexture("textures/map/floor_0.png", false, "floor_0");
	ResourceManager::LoadTexture("textures/map/floor_1.png", false, "floor_1");
	ResourceManager::LoadTexture("textures/map/floor_2.png", false, "floor_2");
	ResourceManager::LoadTexture("textures/map/floor_3.png", false, "floor_3");
	ResourceManager::LoadTexture("textures/map/floor_4.png", false, "floor_4");
	ResourceManager::LoadTexture("textures/map/floor_5.png", false, "floor_5");
	ResourceManager::LoadTexture("textures/map/floor_6.png", false, "floor_6");
	ResourceManager::LoadTexture("textures/map/floor_7.png", false, "floor_7");
	#pragma endregion
	#pragma region Space
	ResourceManager::LoadTexture("textures/map/space_0.png", false, "space_0");
	ResourceManager::LoadTexture("textures/map/space_1.png", false, "space_1");
	ResourceManager::LoadTexture("textures/map/space_2.png", false, "space_2");
	ResourceManager::LoadTexture("textures/map/space_3.png", false, "space_3");
	#pragma endregion
	#pragma region Walls
	ResourceManager::LoadTexture("textures/map/wall_0.png", false, "wall_0");
	ResourceManager::LoadTexture("textures/map/wall_1.png", false, "wall_1");
	ResourceManager::LoadTexture("textures/map/wall_2.png", false, "wall_2");
	ResourceManager::LoadTexture("textures/map/wall_3.png", false, "wall_3");
	ResourceManager::LoadTexture("textures/map/wall_4.png", false, "wall_4");
	ResourceManager::LoadTexture("textures/map/wall_5.png", false, "wall_5");
	ResourceManager::LoadTexture("textures/map/wall_5_0.png", false, "wall_5_0");
	ResourceManager::LoadTexture("textures/map/wall_5_1.png", false, "wall_5_1");
	ResourceManager::LoadTexture("textures/map/wall_5_2.png", false, "wall_5_2");
	ResourceManager::LoadTexture("textures/map/wall_5_3.png", false, "wall_5_3");
	ResourceManager::LoadTexture("textures/map/wall_6.png", false, "wall_6");
	ResourceManager::LoadTexture("textures/map/wall_7.png", false, "wall_7");
	ResourceManager::LoadTexture("textures/map/wall_8.png", false, "wall_8");
	ResourceManager::LoadTexture("textures/map/wall_9.png", false, "wall_9");
	ResourceManager::LoadTexture("textures/map/wall_10.png", false, "wall_10");
	ResourceManager::LoadTexture("textures/map/wall_10_0.png", false, "wall_10_0");
	ResourceManager::LoadTexture("textures/map/wall_10_1.png", false, "wall_10_1");
	ResourceManager::LoadTexture("textures/map/wall_10_2.png", false, "wall_10_2");
	ResourceManager::LoadTexture("textures/map/wall_10_3.png", false, "wall_10_3");
	ResourceManager::LoadTexture("textures/map/wall_10_4.png", false, "wall_10_4");
	ResourceManager::LoadTexture("textures/map/wall_10_5.png", false, "wall_10_5");
	ResourceManager::LoadTexture("textures/map/wall_11.png", false, "wall_11");
	ResourceManager::LoadTexture("textures/map/wall_12.png", false, "wall_12");
	ResourceManager::LoadTexture("textures/map/wall_13.png", false, "wall_13");
	ResourceManager::LoadTexture("textures/map/wall_14.png", false, "wall_14");
	ResourceManager::LoadTexture("textures/map/wall_15.png", false, "wall_15");
	ResourceManager::LoadTexture("textures/map/wall_single.png", false, "wall_single");
	#pragma endregion
	#pragma region Decals
	ResourceManager::LoadTexture("textures/decals/aoe.png", true, "aoe");
	ResourceManager::LoadTexture("textures/decals/attack.png", true, "attack");
	ResourceManager::LoadTexture("textures/decals/heal.png", true, "heal");
	ResourceManager::LoadTexture("textures/decals/melee.png", true, "melee");
	ResourceManager::LoadTexture("textures/decals/cobweb_3.png", true, "cobweb_3");
	ResourceManager::LoadTexture("textures/decals/cobweb_6.png", true, "cobweb_6");
	ResourceManager::LoadTexture("textures/decals/cobweb_9.png", true, "cobweb_9");
	ResourceManager::LoadTexture("textures/decals/cobweb_12.png", true, "cobweb_12");
	ResourceManager::LoadTexture("textures/decals/wall_shadow.png", true, "wall_shadow");
	#pragma endregion

	// Randomize tiles
	std::normal_distribution<double> normalDistribution(0.0, 1.0);
	int tilesOnScreen = (SCREEN_HEIGHT / SPRITE_SIZE) * (SCREEN_WIDTH / SPRITE_SIZE);
	for (int i = 0; i < tilesOnScreen; i++) {
		pFloorVariations->push_back(sample(std::abs(std::min(normalDistribution(HunterKillerConstants::RNG), 2.0)), 8));
		pFloorDecorations->push_back(sample(std::abs(std::min(normalDistribution(HunterKillerConstants::RNG), 2.0)), 4));
		pSpaceVariations->push_back(sample(std::abs(std::min(normalDistribution(HunterKillerConstants::RNG), 2.0)), 4));
		pWallVerticalVariations->push_back(sample(std::abs(std::min(normalDistribution(HunterKillerConstants::RNG), 2.0)), 4));
		pWallHorizontalVariations->push_back(sample(std::abs(std::min(normalDistribution(HunterKillerConstants::RNG), 2.0)), 6));
	}
}

void Render(HunterKillerState* pState, HunterKillerAction* pAction)
{
	auto& rMap = pState->GetMap();
	auto& rMapContent = rMap.GetMapContent();
	for (int i = 0; i < rMapContent.size(); i++)
	{
		auto* pMapFeature = dynamic_cast<MapFeature*>(rMapContent[i].at(HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX));
		auto& rMapLocation = rMap.ToLocation(i);
		int x = rMapLocation.GetX() * SPRITE_SIZE + (SCREEN_WIDTH - MAP_WIDTH) / 2;
		int y = rMapLocation.GetY() * SPRITE_SIZE + (SCREEN_HEIGHT - MAP_HEIGHT) / 2;

		switch (pMapFeature->GetType()) {
		case FLOOR: {
			int wallMask = determineWallMask(rMap, pMapFeature->GetLocation());
			pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("floor_{0}", pFloorVariations->at(i))), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			// Cobwebs can only be shown on wallMasks 3, 6, 9, 12
			if ((wallMask == 3 || wallMask == 6 || wallMask == 9 || wallMask == 12)&& pFloorDecorations->at(i) > 0)
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("cobweb_{0}", wallMask)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			// Floors can have shadow from Walls in some cases
			auto* pFeatureUpNorth = rMap.GetFeatureAtLocation(*rMap.GetLocationInDirection(rMapLocation, NORTH, 1));
			if ((wallMask & UP_MASK) != 0 && (dynamic_cast<Wall*>(pFeatureUpNorth) || (dynamic_cast<Door*>(pFeatureUpNorth) && !dynamic_cast<Door*>(pFeatureUpNorth)->IsOpen())))
				pRenderer->DrawSprite(ResourceManager::GetTexture("wall_shadow"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			break;
		}
		case WALL: {
			int wallMask = determineWallMask(rMap, pMapFeature->GetLocation());
			if (wallMask != 5 && wallMask != 10) {
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("wall_{0}", wallMask)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				break;
			}
			int variation = wallMask == 5 ? pWallVerticalVariations->at(i) : pWallHorizontalVariations->at(i);
			pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("wall_{0}_{1}", wallMask, variation)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			break;
		}
		case DOOR_CLOSED: {
			int wallMask = determineWallMask(rMap, pMapFeature->GetLocation());
			float doorRotation = wallMask == 5 ? 90.0f : 0.0f;
			pRenderer->DrawSprite(ResourceManager::GetTexture("door_closed"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), doorRotation, COLOR_WHITE);
			break;
		}
		case DOOR_OPEN: {
			// Open doors need a Floor as background
			pRenderer->DrawSprite(ResourceManager::GetTexture("floor_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			int wallMask = determineWallMask(rMap, pMapFeature->GetLocation());
			float doorRotation = wallMask == 5 ? 90.0f : 0.0f;
			pRenderer->DrawSprite(ResourceManager::GetTexture("door_open"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), doorRotation, COLOR_WHITE);
			int time = dynamic_cast<Door*>(pMapFeature)->GetOpenTimer();
			pText->RenderText(std::format("{0:d}", time), x + TEXT_OFFSET, y + (SPRITE_SIZE - TEXT_OFFSET), 0.4f, COLOR_CYAN);
			break;
		}
		case SPACE:
			pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("space_{0}", pSpaceVariations->at(i))), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
			break;
		default:
			// We're dealing with a Structure
			auto* pStructure = dynamic_cast<Structure*>(pMapFeature);
			switch (pStructure->GetStructureType()) {
			case STRUCTURE_BASE:
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("base_p{0}_0", pStructure->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				pText->RenderText(std::format("{0:d}", pState->GetPlayer(pStructure->GetControllingPlayerID())->GetResource()), x * 1.0f, y * 1.0f, 0.4f, COLOR_CYAN);
				break;
			case STRUCTURE_OUTPOST:
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("base_p{0}_1", !pStructure->IsUnderControl() ? 5 : pStructure->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				break;
			// Rest of the structures need a floor as background
			case STRUCTURE_STRONGHOLD:
				pRenderer->DrawSprite(ResourceManager::GetTexture("floor_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("base_p{0}_2", !pStructure->IsUnderControl() ? 5 : pStructure->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				break;
			case STRUCTURE_OBJECTIVE:
				pRenderer->DrawSprite(ResourceManager::GetTexture("floor_0"), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("base_p{0}_3", !pStructure->IsUnderControl() ? 5 : pStructure->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
				break;
			}
			pText->RenderText(std::format("{0:d}", pStructure->GetCurrentHP()), x * 1.0f, y + (SPRITE_SIZE - TEXT_OFFSET), 0.4f, COLOR_RED);
			break;
		}
		
		auto* pUnit = dynamic_cast<Unit*>(rMapContent[i].at(HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX));
		if (pUnit) {
			// Since our Unit's sprites are originally facing WEST, other orientations need mirroring or rotation.
			bool mirror = false;
			float unitRotation = 0.0f;
			switch (pUnit->GetOrientation()) {
			case NORTH:
				x = x + 4;
				unitRotation = 90.0f;
				break;
			case SOUTH:
				x = x - 4;
				unitRotation = 270.0f;
				break;
			case EAST:
				y = y - 4;
				mirror = true;
				break;
			case WEST:
				y = y - 4;
				break;
			}
			switch (pUnit->GetType()) {
			case UNIT_INFECTED:
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("infected_p{0}_0", pUnit->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), unitRotation, COLOR_WHITE, mirror);
				break;
			case UNIT_MEDIC:
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("medic_p{0}_0", pUnit->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), unitRotation, COLOR_WHITE, mirror);
				break;
			case UNIT_SOLDIER:
				pRenderer->DrawSprite(ResourceManager::GetTexture(std::format("soldier_p{0}_0", pUnit->GetControllingPlayerID() + 1)), glm::vec2(x * 1.0f, y * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), unitRotation, COLOR_WHITE, mirror);
				break;
			}

			if (pUnit->GetCurrentHP() < pUnit->GetMaxHP())
				pText->RenderText(std::format("{0:d}", pUnit->GetCurrentHP()), x * 1.0f, y + (SPRITE_SIZE - TEXT_OFFSET), 0.4f, COLOR_RED);
			if (pUnit->GetSpecialAttackCooldown() > 0)
				pText->RenderText(std::format("{0:d}", pUnit->GetSpecialAttackCooldown()), x + (SPRITE_SIZE - TEXT_OFFSET), y + (SPRITE_SIZE - TEXT_OFFSET), 0.4f, COLOR_CYAN);
		}
	}

	if (pAction) {
		for (auto* pOrder : *(pAction->GetOrders())) {
			UnitOrder* pUnitOrder = dynamic_cast<UnitOrder*>(pOrder);
			if (pUnitOrder) {
				glm::vec3 orderTextColor = pOrder->IsAccepted() ? COLOR_GREEN : COLOR_PINK;
				UnitOrderType type = pUnitOrder->GetOrderType();
				UnitType actorType = pUnitOrder->GetUnitType();
				std::optional<MapLocation> oTarget = pUnitOrder->GetTargetLocation();
				
				auto* pActor = rMap.GetObject(pUnitOrder->GetObjectID());
				if (pActor && dynamic_cast<Unit*>(pActor)) {
					auto* pUnit = dynamic_cast<Unit*>(pActor);
					auto& rLocation = pUnit->GetLocation();
					int x = rLocation.GetX() * SPRITE_SIZE + (2 * SPRITE_SIZE / 3) - TEXT_OFFSET + (SCREEN_WIDTH - MAP_WIDTH) / 2;
					int y = rLocation.GetY() * SPRITE_SIZE + SPRITE_SIZE - TEXT_OFFSET + (SCREEN_HEIGHT - MAP_HEIGHT) / 2;
					
					pText->RenderText(std::format("{0:d}", (int)type), x, y, 0.4f, orderTextColor);
				}
				
				if (oTarget.has_value() && rMap.IsOnMap(oTarget.value())) {
					int targetX = oTarget.value().GetX() * SPRITE_SIZE + (SCREEN_WIDTH - MAP_WIDTH) / 2;
					int targetY = oTarget.value().GetY() * SPRITE_SIZE + (SCREEN_HEIGHT - MAP_HEIGHT) / 2;
					if (type == ATTACK) {
						switch (actorType) {
						case UNIT_INFECTED:
							pRenderer->DrawSprite(ResourceManager::GetTexture("melee"), glm::vec2(targetX * 1.0f, targetY * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
							break;
						case UNIT_MEDIC:
						case UNIT_SOLDIER:
							pRenderer->DrawSprite(ResourceManager::GetTexture("attack"), glm::vec2(targetX * 1.0f, targetY * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
							break;
						}
					} else if (type == ATTACK_SPECIAL) {
						switch (actorType) {
						case UNIT_INFECTED:
							break;
						case UNIT_MEDIC:
							pRenderer->DrawSprite(ResourceManager::GetTexture("heal"), glm::vec2(targetX * 1.0f, targetY * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
							break;
						case UNIT_SOLDIER:
							auto* pSoldierAOE = new std::unordered_set<MapLocation, MapLocationHash>();
							rMap.GetAreaAround(oTarget.value(), true, *pSoldierAOE);
							for (auto& rLocation : *pSoldierAOE) {
								MapFeature* pFeature = rMap.GetFeatureAtLocation(rLocation);
								if (dynamic_cast<Wall*>(pFeature))
									continue;
								int aoeX = rLocation.GetX() * SPRITE_SIZE + (SCREEN_WIDTH - MAP_WIDTH) / 2;
								int aoeY = rLocation.GetY() * SPRITE_SIZE + (SCREEN_HEIGHT - MAP_HEIGHT) / 2;
								pRenderer->DrawSprite(ResourceManager::GetTexture("aoe"), glm::vec2(aoeX * 1.0f, aoeY * 1.0f), glm::vec2(SPRITE_SIZE * 1.0f, SPRITE_SIZE * 1.0f), 0.0f, COLOR_WHITE);
							}
							delete pSoldierAOE; pSoldierAOE = nullptr;
							break;
						}
					}
				}
			}
		}
	}
		
    // render text (don't include in postprocessing)
	pText->RenderText(std::format("Player 1 score: {0:d} --- Player 2 score: {1:d}", pState->GetPlayer(0)->GetScore(), pState->GetPlayer(1)->GetScore()), 5.0f, 5.0f, 0.5f, COLOR_UI_TEXT);
}

/** Returns whether the feature at the given index in the adjacency matrix contains a Wall or Door. */
bool isWalled(std::vector<std::vector<MapFeature*>>& rFeatures, int x, int y) {
	return rFeatures[y].at(x) && (dynamic_cast<Wall*>(rFeatures[y].at(x)) || dynamic_cast<Door*>(rFeatures[y].at(x)));
}

int determineWallMask(HunterKillerMap& rMap, MapLocation& rLocation) {
	auto* pFeatures = new std::vector<std::vector<MapFeature*>>();
	pFeatures->resize(3);
	for (std::vector<MapFeature*>& rFeaturesRow : *pFeatures) {	
		rFeaturesRow.resize(3);
	}
	rMap.GetMapFeaturesAround(rLocation, *pFeatures);
	int wallMask = isWalled(*pFeatures, 1, 0) ? UP_MASK : 0;
	wallMask += isWalled(*pFeatures, 2, 1) ? RIGHT_MASK : 0;
	wallMask += isWalled(*pFeatures, 1, 2) ? DOWN_MASK : 0;
	wallMask += isWalled(*pFeatures, 0, 1) ? LEFT_MASK : 0;
	delete pFeatures; pFeatures = nullptr;
	return wallMask;
}

/** Grabs all regions from the skin under the key and samples from them with the current weight. */
int sample(double weight, int collectionSize) {
	// Simple stop once we sample on the weight.
	for (int i = 0; i < collectionSize; i++) {
		if (weight < 1 + i * 0.2f)
			return i;
	}
	return 0;
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