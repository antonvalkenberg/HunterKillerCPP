#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKillerBots/BaseBot.h"
#include "../HunterKillerBots/QuickRandomBot.h"

int main()
{
	auto* actions = new std::vector<HunterKillerAction*>();
	auto* actionResults = new std::vector<std::string>();
	auto* factory = new HunterKillerStateFactory();
	auto* player1Name = new std::string("A");
	auto* player2Name = new std::string("B");
    const auto* playerNames = new std::vector{ player1Name, player2Name };

	HunterKillerState* state = factory->GenerateInitialState(*playerNames);

	auto* bot = new QuickRandomBot();

	Result* result;
	do {
        const HunterKillerState* stateCopy = state->Copy();
		//stateCopy.prepare(state.getActivePlayerID());

		HunterKillerAction* action = bot->Handle(*stateCopy);
		actions->push_back(action);

		result = HunterKillerRules::Handle(*state, *action);

		if (!result->Information->empty())
			actionResults->push_back(std::string(*result->Information));

		std::cout << state->GetMap().ToString() << std::endl;

		std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (!result->FinishedGame && result->Accepted);

	return 0;
}