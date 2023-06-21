#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKillerBots/BaseBot.h"
#include "../HunterKillerBots/QuickRandomBot.h"
#include "../HunterKillerBots/RandomBot.h"

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// The Width of the screen
const unsigned int SCREEN_WIDTH = 800;
// The height of the screen
const unsigned int SCREEN_HEIGHT = 600;

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
		//TODO: call game Render

		glfwSwapBuffers(window);

		delete pStateCopy; pStateCopy = nullptr;
		delete pResult; pResult = nullptr;
	} while (!finishedGame && !glfwWindowShouldClose(window));

	glfwTerminate();
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