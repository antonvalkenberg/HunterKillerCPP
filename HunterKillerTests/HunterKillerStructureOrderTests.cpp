#include "pch.h"
#include "../HunterKiller/HelperStructs.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/StructureOrder.h"
#include "../HunterKiller/Infected.h"

class StructureOrderTest : public testing::Test
{
protected:
    void SetUp() override
    {
		TestMapSetup = new MapSetup(*TestMapData);
		Factory = new HunterKillerStateFactory();
		State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *TestMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);
	}
    void TearDown() override
    {
		delete State; State = nullptr;
		delete Factory; Factory = nullptr;
		for (auto* pName : *PlayerNames)
		{
			delete pName; pName = nullptr;
		}
		delete PlayerNames; PlayerNames = nullptr;
		PlayerAName = nullptr;
		PlayerBName = nullptr;
		delete TestMapData; TestMapData = nullptr;
		delete TestMapSetup; TestMapSetup = nullptr;
    }
	std::string* PlayerAName = new std::string("playerA");
	std::string* PlayerBName = new std::string("playerB");
	std::vector<std::string*>* PlayerNames = new std::vector{ PlayerAName, PlayerBName };
	const std::string* TestMapData = new std::string("B__\n___");
	MapSetup* TestMapSetup = nullptr;
	HunterKillerStateFactory* Factory = nullptr;
	HunterKillerState* State = nullptr;
};

TEST_F(StructureOrderTest, TestSpawn)
{
	// Set some values of things before the order
    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const int beforePlayerResource = rActivePlayer.GetResource();
    const int beforePlayerSquadSize = static_cast<int>(rActivePlayer.GetUnitIDs().size());
    const Structure* pBase = dynamic_cast<Structure*>(State->GetMap().GetObject(rActivePlayer.GetCommandCenterID()));
    const MapLocation* pSpawnLocation = pBase->GetSpawnLocation();

	// Create a base-order to spawn an infected for the active player
    const HunterKillerAction* pSpawnInfectedAction = new HunterKillerAction(*State);
	StructureOrder* pOrder = StructureOrder::Spawn(*pBase, UNIT_INFECTED);
	pSpawnInfectedAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpawnInfectedAction);

	// Check that the action was accepted
	ASSERT_TRUE(pResult->Accepted);
	// Check that the action did not result in a finished state
	ASSERT_TRUE(!State->IsDone());
	// Check that the failure explanations are empty
	ASSERT_TRUE(pResult->Information->empty());

	// Check that the player's resource was reduced
	ASSERT_EQ(beforePlayerResource - HunterKillerConstants::INFECTED_SPAWN_COST, rActivePlayer.GetResource());
	// Check that the player has an extra squad member
	ASSERT_EQ(beforePlayerSquadSize + 1, rActivePlayer.GetUnitIDs().size());

	// Check that there is an infected on the map, in the correct location
	ASSERT_TRUE(dynamic_cast<Infected*>(State->GetMap().GetUnitAtLocation(*pSpawnLocation)));
	// Check that the infected on the spawn location belongs to the formerly active player
	ASSERT_EQ(rActivePlayer.GetID(), State->GetMap().GetUnitAtLocation(*pSpawnLocation)->GetControllingPlayerID());

	// Get the newly spawned unit
    const Unit* pSpawnedUnit = State->GetMap().GetUnitAtLocation(*pSpawnLocation);
	// Get the current combined FoV for the player
    const auto* pPlayerFoV = rActivePlayer.GetCombinedFieldOfView(State->GetMap());

	// Check that each location that the new Infected can see, is also in the player's current combined FoV.
	auto* pUnitFoV = State->GetMap().GetFieldOfView(*pSpawnedUnit);
	for (auto it = pUnitFoV->begin(); it != pUnitFoV->end(); ++it)
	{
		ASSERT_TRUE(pPlayerFoV->contains(*it));
	}

	pUnitFoV = nullptr; // Units delete their own field-of-view
	delete pPlayerFoV; pPlayerFoV = nullptr;
	delete pResult; pResult = nullptr;
	delete pSpawnInfectedAction; pSpawnInfectedAction = nullptr;
	pSpawnedUnit = nullptr;
	pOrder = nullptr;
	pSpawnLocation = nullptr;
    pBase = nullptr;
}

TEST_F(StructureOrderTest, TestFailSpawnLowResource)
{
	// Set some values of things before the order
	HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const Structure* pBase = dynamic_cast<Structure*>(State->GetMap().GetObject(rActivePlayer.GetCommandCenterID()));
    const MapLocation* pSpawnLocation = pBase->GetSpawnLocation();

	// Create a base-order to spawn an infected for the active player
    const HunterKillerAction* pSpawnInfectedAction = new HunterKillerAction(*State);
	StructureOrder* pOrder = StructureOrder::Spawn(*pBase, UNIT_INFECTED);
	pSpawnInfectedAction->TryAddOrder(pOrder);

	// Now set the player's resource to an amount that is not enough to spawn an infected unit
	rActivePlayer.SetResource(0);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpawnInfectedAction);

	// Check that there is a failure message
	ASSERT_FALSE(pResult->Information->empty());

	// Check that no unit is at the spawn location
	ASSERT_FALSE(State->GetMap().GetUnitAtLocation(*pSpawnLocation));

	delete pResult; pResult = nullptr;
	delete pSpawnInfectedAction; pSpawnInfectedAction = nullptr;
	pOrder = nullptr;
	pSpawnLocation = nullptr;
	pBase = nullptr;
}

TEST_F(StructureOrderTest, TestFailSpawnLocationOccupied)
{
	// Re-create the State with the map that is setup to fail.
	const std::string* pFailMapData = new std::string("B__\nS__");
	auto* pFailMapSetup = new MapSetup(*pFailMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pFailMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

	// Set some values of things before the order
    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const int pre_PlayerSquadSize = static_cast<int>(rActivePlayer.GetUnitIDs().size());
    const int pre_PlayerResources = rActivePlayer.GetResource();
    const Structure* pBase = dynamic_cast<Structure*>(State->GetMap().GetObject(rActivePlayer.GetCommandCenterID()));

	// Create a base-order to spawn an infected for the active player
    const HunterKillerAction* pSpawnInfectedAction = new HunterKillerAction(*State);
	StructureOrder* pOrder = StructureOrder::Spawn(*pBase, UNIT_INFECTED);
	pSpawnInfectedAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpawnInfectedAction);
	
	// Check that there is a failure message
	ASSERT_FALSE(pResult->Information->empty());
	// Check that nothing has actually been spawned
	ASSERT_EQ(pre_PlayerSquadSize, rActivePlayer.GetUnitIDs().size());
	ASSERT_EQ(pre_PlayerResources, rActivePlayer.GetResource());

	delete pResult; pResult = nullptr;
	delete pSpawnInfectedAction; pSpawnInfectedAction = nullptr;
	pOrder = nullptr;
	pBase = nullptr;
	delete pFailMapSetup; pFailMapSetup = nullptr;
	delete pFailMapData; pFailMapData = nullptr;
}