#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKillerBots/BaseBot.h"
#include "../HunterKillerBots/QuickRandomBot.h"
#include "../HunterKillerBots/RandomBot.h"

int main()
{
	auto* pActions = new std::vector<HunterKillerAction*>();
	auto* pActionResults = new std::vector<std::string>();
	auto* pFactory = new HunterKillerStateFactory();
	auto* pPlayer1Name = new std::string("A");
	auto* pPlayer2Name = new std::string("B");
    const auto* pPlayerNames = new std::vector{ pPlayer1Name, pPlayer2Name };

	HunterKillerState* pState = pFactory->GenerateInitialState(*pPlayerNames);

	auto* bot = new RandomBot();

	bool finishedGame;
	do {
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

		delete pStateCopy; pStateCopy = nullptr;
		delete pResult; pResult = nullptr;
	} while (!finishedGame);

	return 0;
}