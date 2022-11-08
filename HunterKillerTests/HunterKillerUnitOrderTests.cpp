#include "pch.h"
#include "../HunterKiller/HelperStructs.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/UnitOrder.h"
#include "../HunterKiller/Soldier.h"
#include "../HunterKiller/Infected.h"

class UnitOrderTest : public testing::Test
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
	const std::string* TestMapData = new std::string("B_S\n___");
	MapSetup* TestMapSetup = nullptr;
	HunterKillerStateFactory* Factory = nullptr;
	HunterKillerState* State = nullptr;
};

TEST_F(UnitOrderTest, TestUnitRotation)
{
    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const HunterKillerMap& rMap = State->GetMap();
	// Select the unit
    const Unit* pUnit = dynamic_cast<Unit*>(rMap.GetObject(rActivePlayer.GetUnitIDs().at(0)));

	// Situation before rotation:
	// B _ S _ _ _
	// _ _ _ _ _ _
	// _ _ _ _ _ _
	// _ _ _ S _ B
    const Direction pre_UnitOrientation = pUnit->GetOrientation();
    const auto* pPre_UnitFoV = pUnit->GetFieldOfView();
	// Make sure we are assuming the right things about our unit
	ASSERT_TRUE(pre_UnitOrientation == NORTH);
	ASSERT_EQ(1, pPre_UnitFoV->size());
	ASSERT_TRUE(pPre_UnitFoV->contains(rMap.ToLocation(2, 0)));

	// Create an order to rotate the unit
    const HunterKillerAction* pRotateAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::RotateUnit(*pUnit, true);
	pRotateAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pRotateAction);

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

    const Direction post_UnitOrientation = pUnit->GetOrientation();
	auto* pPost_UnitFoV = pUnit->GetFieldOfView();

	// Check that the unit has the correct orientation
	ASSERT_TRUE(post_UnitOrientation == EAST);
	// Check that the unit's field-of-view has changed
	ASSERT_EQ(6, pPost_UnitFoV->size());
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(2, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(3, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(4, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(5, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(3, 1)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(4, 1)));

	// Check that the player's combined field-of-view now also contains these locations
    const auto* pPlayerFoV = rActivePlayer.GetCombinedFieldOfView(State->GetMap());
	for (auto it = pPost_UnitFoV->begin(); it != pPost_UnitFoV->end(); ++it)
	{
		ASSERT_TRUE(pPlayerFoV->contains(*it));
	}

	delete pPlayerFoV; pPlayerFoV = nullptr;
	pPost_UnitFoV = nullptr;
	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pRotateAction; pRotateAction = nullptr;
	pPre_UnitFoV = nullptr;
	pUnit = nullptr;
}

TEST_F(UnitOrderTest, TestUnitMovement)
{
    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rPre_UnitLocation = rMap.ToLocation(2, 0);
    const MapLocation& rPost_UnitLocation = rMap.ToLocation(2, 1);
	// Select the unit
    const Unit* pUnit = rMap.GetUnitAtLocation(rPre_UnitLocation);

	// Situation before movement:
	// B _ S _ _ _
	// _ _ _ _ _ _
	// _ _ _ _ _ _
	// _ _ _ S _ B

	// Create an order to move the unit
    const HunterKillerAction* pMoveAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::MoveUnit(*pUnit, *rMap.GetAdjacentLocationInDirection(rPre_UnitLocation, SOUTH));
	pMoveAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pMoveAction);

	// Situation after movement:
	// B _ _ _ _ _
	// _ _ S _ _ _
	// _ _ _ _ _ _
	// _ _ _ S _ B

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Get the unit's new FoV
	auto* pPost_UnitFoV = pUnit->GetFieldOfView();

	// Check that the unit is at the target location
	ASSERT_EQ(rPost_UnitLocation, pUnit->GetLocation());
	// Check that there is no unit at the old location
	ASSERT_FALSE(rMap.GetUnitAtLocation(rPre_UnitLocation));

	// Check that the unit's Field-of-View has changed
	ASSERT_EQ(4, pPost_UnitFoV->size());
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(2, 1)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(1, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(2, 0)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(3, 0)));

	// Check that the player's combined field-of-view now also contains these locations
    const auto* pPlayerFoV = rActivePlayer.GetCombinedFieldOfView(State->GetMap());
	for (auto it = pPost_UnitFoV->begin(); it != pPost_UnitFoV->end(); ++it)
	{
		ASSERT_TRUE(pPlayerFoV->contains(*it));
	}

	delete pPlayerFoV; pPlayerFoV = nullptr;
	pPost_UnitFoV = nullptr;
	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pMoveAction; pMoveAction = nullptr;
	pUnit = nullptr;
}

TEST_F(UnitOrderTest, TestMovementThroughDoor)
{
	// Re-create the map using the door setup
	const std::string* pTestDoorMapData = new std::string("B___\n_S__\n|D||\n|__|");
	auto* pDoorMapSetup = new MapSetup(*pTestDoorMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pDoorMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(1, 1);
    const MapLocation& rDoorLocation = rMap.ToLocation(1, 2);

	// Units created in the initial state are facing NORTH, but we want to test here with it facing SOUTH, so change
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	pUnit->SetOrientation(SOUTH);
    const auto* pPre_UnitFoV = pUnit->GetFieldOfView();
	// Get the door that we want to check.
    const Door* pDoor = dynamic_cast<Door*>(rMap.GetFeatureAtLocation(rDoorLocation));

	// Situation before movement:
	// B _ _ _ _ _ _ _
	// _ S _ _ _ _ _ _
	// | D | | | | _ |
	// | _ _ | | _ _ |
	// | _ _ | | _ _ |
	// | _ | | | | D |
	// _ _ _ _ _ _ S _
	// _ _ _ _ _ _ _ B
	ASSERT_FALSE(pDoor->IsOpen());
	ASSERT_EQ(4, pPre_UnitFoV->size());

	// Create an order to move the unit
    const HunterKillerAction* pMoveAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::MoveUnit(*pUnit, *rMap.GetAdjacentLocationInDirection(pUnit->GetLocation(), SOUTH));
	pMoveAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pMoveAction);

	// Situation after movement:
	// B _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// | S | | | | _ |
	// | _ _ | | _ _ |
	// | _ _ | | _ _ |
	// | _ | | | | D |
	// _ _ _ _ _ _ S _
	// _ _ _ _ _ _ _ B

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Check if the door opened
	ASSERT_TRUE(pDoor->IsOpen());
	ASSERT_EQ(HunterKillerConstants::DOOR_OPEN_ROUNDS, pDoor->GetOpenTimer());
	
	// Get the unit's new FoV
	auto* pPost_UnitFoV = pUnit->GetFieldOfView();

	// Check that the unit's Field-of-View has changed
	ASSERT_EQ(8, pPost_UnitFoV->size());
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(1, 2)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(0, 3)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(1, 3)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(2, 3)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(0, 4)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(1, 4)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(2, 4)));
	ASSERT_TRUE(pPost_UnitFoV->contains(rMap.ToLocation(1, 5)));

	// Check that the player's combined field-of-view now also contains these locations
    const auto* pPlayerFoV = rActivePlayer.GetCombinedFieldOfView(State->GetMap());
	for (auto it = pPost_UnitFoV->begin(); it != pPost_UnitFoV->end(); ++it)
	{
		ASSERT_TRUE(pPlayerFoV->contains(*it));
	}

	delete pPlayerFoV; pPlayerFoV = nullptr;
	pPost_UnitFoV = nullptr;
	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pMoveAction; pMoveAction = nullptr;
	pDoor = nullptr;
	pPre_UnitFoV = nullptr;
	pUnit = nullptr;
	delete pDoorMapSetup; pDoorMapSetup = nullptr;
	delete pTestDoorMapData; pTestDoorMapData = nullptr;
}

TEST_F(UnitOrderTest, TestFailMovementBlocked)
{
	// Re-create the map using the blocked setup
    const std::string* pTestBlockedMapData = new std::string("B_S\n__S");
	auto* pBlockedMapSetup = new MapSetup(*pTestBlockedMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pBlockedMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

    const HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rPre_UnitLocation = rMap.ToLocation(2, 0);
    const MapLocation& rTargetLocation = rMap.ToLocation(2, 1);

    const Unit* pUnit = rMap.GetUnitAtLocation(rPre_UnitLocation);
    const int unitID = pUnit->GetID();
    const Unit* pUnitAtTarget = rMap.GetUnitAtLocation(rTargetLocation);
    const int unitAtTargetID = pUnitAtTarget->GetID();

	// Situation before movement:
	// B _ S _ _ _
	// _ _ S _ _ _
	// _ _ _ S _ _
	// _ _ _ S _ B

	// Create an order to move the unit
    const HunterKillerAction* pMoveAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::MoveUnit(*pUnit, *rMap.GetAdjacentLocationInDirection(pUnit->GetLocation(), SOUTH));
	pMoveAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pMoveAction);

	// Check that there is a fail message
	ASSERT_FALSE(pResult->Information->empty());

	// Check that there is still a unit on the location
	Unit* pPost_Unit = rMap.GetUnitAtLocation(rPre_UnitLocation);
    ASSERT_TRUE(pPost_Unit);
	
	// Check that it is still the same unit at the location (and the target location)
	ASSERT_EQ(unitID, pPost_Unit->GetID());
	ASSERT_EQ(unitAtTargetID, pUnitAtTarget->GetID());

	pPost_Unit = nullptr;
	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pMoveAction; pMoveAction = nullptr;
	pUnitAtTarget = nullptr;
	pUnit = nullptr;
	delete pBlockedMapSetup; pBlockedMapSetup = nullptr;
	delete pTestBlockedMapData; pTestBlockedMapData = nullptr;
}

TEST_F(UnitOrderTest, TestUnitAttack)
{
	// Re-create the map using the smaller map for attacking
    const std::string* pTestAttackMapData = new std::string("BS");
	auto* pAttackMapSetup = new MapSetup(*pTestAttackMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pAttackMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

	HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(1, 0);
	const MapLocation& rTargetLocation = rMap.ToLocation(2, 1);

	// Units created in the initial state are facing NORTH, but we want to test here with it facing EAST, so change
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	pUnit->SetOrientation(EAST);
	pUnit->InvalidateFieldOfView();
	// Make sure the map updates the field-of-view, because we changed a unit's orientation.
	// Note: this is normally handled by the HunterKillerRules if executed through an order, but we skipped that.
	rMap.UpdateFieldOfView();

	// Get some data about our target
    const Unit* pTargetUnit = rMap.GetUnitAtLocation(rTargetLocation);
    const int pre_TargetUnitHealth = pTargetUnit->GetCurrentHP();

	// Situation before attack:
	// B S _ _
	// _ _ S B

	// Create an order to attack the target location
    const HunterKillerAction* pAttackAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::UnitAttack(*pUnit, rTargetLocation, false);
	pAttackAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pAttackAction);

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Refresh target's data
    const int post_TargetUnitHealth = pTargetUnit->GetCurrentHP();
	// Make sure health was lost, and the correct amount
	ASSERT_TRUE(post_TargetUnitHealth < pre_TargetUnitHealth);
	ASSERT_EQ(HunterKillerConstants::SOLDIER_ATTACK_DAMAGE, pre_TargetUnitHealth - post_TargetUnitHealth);

	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pAttackAction; pAttackAction = nullptr;
	pTargetUnit = nullptr;
	pUnit = nullptr;
	delete pAttackMapSetup; pAttackMapSetup = nullptr;
	delete pTestAttackMapData; pTestAttackMapData = nullptr;
}

TEST_F(UnitOrderTest, TestSpecialAttackSoldier)
{
	// Re-create the map using the map for the soldier's special attack
    const std::string* pTestSoldierSpecialMapData = new std::string("B__\n__S");
	auto* pSoldierSpecialMapSetup = new MapSetup(*pTestSoldierSpecialMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pSoldierSpecialMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

	HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(2, 1);
    const MapLocation& rTargetLocation = rMap.ToLocation(4, 2);
    const MapLocation& rTargetUnitLocation = rMap.ToLocation(3, 2);
    const MapLocation& rTargetBaseLocation = rMap.ToLocation(5, 3);

	// Units created in the initial state are facing NORTH, but we want to test here with it facing EAST, so change
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	pUnit->SetOrientation(EAST);
	pUnit->InvalidateFieldOfView();
	// Make sure the map updates the field-of-view, because we changed a unit's orientation.
	// Note: this is normally handled by the HunterKillerRules if executed through an order, but we skipped that.
	rMap.UpdateFieldOfView();

	// Get some data about our targets
    const Unit* pTargetUnit = rMap.GetUnitAtLocation(rTargetUnitLocation);
    const int pre_TargetUnitHealth = pTargetUnit->GetCurrentHP();
    const MapFeature* pTargetBase = rMap.GetFeatureAtLocation(rTargetBaseLocation);
    const int pre_TargetBaseHealth = pTargetBase->GetCurrentHP();
    const MapFeature* pTargetFloor = rMap.GetFeatureAtLocation(rTargetLocation);
    const int pre_TargetFloorHealth = pTargetFloor->GetCurrentHP();

	// Situation before attack:
	// B _ _ _ _ _
	// _ _ S _ _ _
	// _ _ _ S _ _
	// _ _ _ _ _ B

	// Create an order to attack the target location
    const HunterKillerAction* pSpecialAttackAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::UnitAttack(*pUnit, rTargetLocation, true);
	pSpecialAttackAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpecialAttackAction);

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Update the status of the objects
    const int post_TargetUnitHealth = pTargetUnit->GetCurrentHP();
    const int post_TargetBaseHealth = pTargetBase->GetCurrentHP();
    const int post_TargetFloorHealth = pTargetFloor->GetCurrentHP();

	// Make sure health was lost, and the correct amount
	ASSERT_TRUE(post_TargetUnitHealth < pre_TargetUnitHealth);
	ASSERT_EQ(HunterKillerConstants::SOLDIER_SPECIAL_DAMAGE, pre_TargetUnitHealth - post_TargetUnitHealth);
	ASSERT_TRUE(post_TargetBaseHealth < pre_TargetBaseHealth);
	ASSERT_EQ(HunterKillerConstants::SOLDIER_SPECIAL_DAMAGE, pre_TargetBaseHealth - post_TargetBaseHealth);

	// Make sure health remains the same for indestructible objects
	ASSERT_EQ(pre_TargetFloorHealth, post_TargetFloorHealth);

	// Make sure the unit that attack has it's special on cooldown
	ASSERT_TRUE(pUnit->GetSpecialAttackCooldown() > 0);

	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pSpecialAttackAction; pSpecialAttackAction = nullptr;
	pTargetFloor = nullptr;
	pTargetBase = nullptr;
	pTargetUnit = nullptr;
	pUnit = nullptr;
	delete pSoldierSpecialMapSetup; pSoldierSpecialMapSetup = nullptr;
	delete pTestSoldierSpecialMapData; pTestSoldierSpecialMapData = nullptr;
}

TEST_F(UnitOrderTest, TestSpecialAttackMedic)
{
	// Re-create the map using the map for the medic's special attack
    const std::string* pTestMedicSpecialMapData = new std::string("BS_M\n____");
	auto* pMedicSpecialMapSetup = new MapSetup(*pTestMedicSpecialMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pMedicSpecialMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

    HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(3, 0);
    const MapLocation& rTargetLocation = rMap.ToLocation(1, 0);

	// Get the Medic we want to give the order to
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	// Units are initially created on the map facing NORTH, but we want our Medic to face WEST towards the Soldier
	pUnit->SetOrientation(WEST);
	pUnit->InvalidateFieldOfView();
	// Make sure the map updates the field-of-view, because we changed a unit's orientation.
	// Note: this is normally handled by the HunterKillerRules if executed through an order, but we skipped that.
	rMap.UpdateFieldOfView();

	// Get the Soldier we want to heal
	Unit* pAffectedUnit = rMap.GetUnitAtLocation(rTargetLocation);
	// Set the Soldier's health to a lower amount, so we can see the effect of the heal
	pAffectedUnit->ReduceHP(pAffectedUnit->GetCurrentHP() - 1);
    const int pre_TargetUnitHealth = pAffectedUnit->GetCurrentHP();

	// Situation before attack:
	// B S _ M _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _
	// _ _ _ _ M _ S B

	// Create an order to attack the target location
    const HunterKillerAction* pSpecialAttackAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::UnitAttack(*pUnit, rTargetLocation, true);
	pSpecialAttackAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpecialAttackAction);

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Update the status of the objects
    const int post_TargetUnitHealth = pAffectedUnit->GetCurrentHP();

	// Make sure health was gained, and the correct amount
	ASSERT_TRUE(post_TargetUnitHealth > pre_TargetUnitHealth);
	ASSERT_EQ(HunterKillerConstants::MEDIC_SPECIAL_HEAL, post_TargetUnitHealth - pre_TargetUnitHealth);

	// Make sure the unit that attack has it's special on cooldown
	ASSERT_TRUE(pUnit->GetSpecialAttackCooldown() > 0);

	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pSpecialAttackAction; pSpecialAttackAction = nullptr;
	pAffectedUnit = nullptr;
	pUnit = nullptr;
	delete pMedicSpecialMapSetup; pMedicSpecialMapSetup = nullptr;
	delete pTestMedicSpecialMapData; pTestMedicSpecialMapData = nullptr;
}

TEST_F(UnitOrderTest, TestFailSpecialAttackInfected)
{
	// Re-create the map using the map for the infected's special attack
    const std::string* pTestInfectedSpecialMapData = new std::string("BI");
	auto* pInfectedSpecialMapSetup = new MapSetup(*pTestInfectedSpecialMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pInfectedSpecialMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

    HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(1, 0);
    const MapLocation& rTargetLocation = rMap.ToLocation(2, 1);

	// Get the Infected we want to give the order to
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	// Move the unit south, since it has an attack range of only 1
	const MapLocation& rNextUnitLocation = rMap.ToLocation(1, 1);
	rMap.Move(rNextUnitLocation, *pUnit, nullptr);
	pUnit->InvalidateFieldOfView();
	// Make sure the map updates the field-of-view, because we changed a unit's position.
	// Note: this is normally handled by the HunterKillerRules if executed through an order, but we skipped that.
	rMap.UpdateFieldOfView();

	// Save the other player's ID
    const Unit* pTargetUnit = rMap.GetUnitAtLocation(rTargetLocation);
    const int pre_TargetUnitPlayerID = pTargetUnit->GetControllingPlayerID();

	// Situation before attack:
	// B _ _ _
	// _ I I B

	// Create an order to attack the target location
    const HunterKillerAction* pSpecialAttackAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::UnitAttack(*pUnit, rTargetLocation, true);
	pSpecialAttackAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpecialAttackAction);

	// Make sure the order was failed (ignored)
	ASSERT_FALSE(pOrder->IsAccepted());
	ASSERT_FALSE(pResult->Information->empty());

	// Make sure the targeted Infected is still owned/controlled by the same player as before
    const int post_TargetUnitPlayerID = pTargetUnit->GetControllingPlayerID();
	ASSERT_EQ(pre_TargetUnitPlayerID, post_TargetUnitPlayerID);

	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pSpecialAttackAction; pSpecialAttackAction = nullptr;
	pTargetUnit = nullptr;
	pUnit = nullptr;
	delete pInfectedSpecialMapSetup; pInfectedSpecialMapSetup = nullptr;
	delete pTestInfectedSpecialMapData; pTestInfectedSpecialMapData = nullptr;
}

TEST_F(UnitOrderTest, TestInfectedTrigger)
{
	// Re-create the map using the map for the infected's special attack
    const std::string* pTestInfectedSpecialMapData = new std::string("BI");
	auto* pInfectedSpecialMapSetup = new MapSetup(*pTestInfectedSpecialMapData);
	delete State; State = nullptr;
	State = Factory->GenerateInitialStateFromSetup(*PlayerNames, *pInfectedSpecialMapSetup, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);

	HunterKillerMap& rMap = State->GetMap();
    const MapLocation& rUnitLocation = rMap.ToLocation(1, 0);
	MapLocation& rTargetLocation = rMap.ToLocation(2, 1);

	// Get the acting player
    const HunterKillerPlayer& rActingPlayer = State->GetActivePlayer();
	// Get the Infected we want to give the order to
	Unit* pUnit = rMap.GetUnitAtLocation(rUnitLocation);
	// Move the unit south, since it has an attack range of only 1
    const MapLocation& rNextUnitLocation = rMap.ToLocation(1, 1);
	rMap.Move(rNextUnitLocation, *pUnit, nullptr);
	pUnit->InvalidateFieldOfView();
	// Make sure the map updates the field-of-view, because we changed a unit's position.
	// Note: this is normally handled by the HunterKillerRules if executed through an order, but we skipped that.
	rMap.UpdateFieldOfView();

	// Get the unit at the target location
	Unit* pTempUnit = rMap.GetUnitAtLocation(rTargetLocation);
	// We need to replace this unit, because an infected's special won't trigger off of another Infected unit
	rMap.UnregisterGameObject(pTempUnit);
	delete pTempUnit; pTempUnit = nullptr;
	// Create a new Soldier, with just enough health to die to an Infected's attack
	auto* pPre_TargetUnit = new Soldier(0, rTargetLocation, HunterKillerConstants::SOLDIER_MAX_HP,
		HunterKillerConstants::INFECTED_ATTACK_DAMAGE, WEST,
		HunterKillerConstants::SOLDIER_FOV_RANGE, HunterKillerConstants::SOLDIER_FOV_ANGLE,
		HunterKillerConstants::SOLDIER_ATTACK_RANGE, HunterKillerConstants::SOLDIER_ATTACK_DAMAGE,
		HunterKillerConstants::SOLDIER_COOLDOWN, HunterKillerConstants::SOLDIER_SPAWN_COST,
		HunterKillerConstants::SOLDIER_SCORE);
	rMap.RegisterGameObject(pPre_TargetUnit);
	rMap.Place(rTargetLocation, pPre_TargetUnit);

	// Situation before attack:
	// B _ _ _
	// _ I S B

	// Create an order to attack the target location
    const HunterKillerAction* pSpecialAttackAction = new HunterKillerAction(*State);
	UnitOrder* pOrder = UnitOrder::UnitAttack(*pUnit, rTargetLocation, false);
	pSpecialAttackAction->TryAddOrder(pOrder);

	// Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pSpecialAttackAction);

	// Make sure the order did not fail
	ASSERT_TRUE(pResult->Accepted);
	ASSERT_TRUE(pResult->Information->empty());

	// Update the status of the objects
	Unit* pPost_TargetUnit = rMap.GetUnitAtLocation(rTargetLocation);

	// Make sure the Unit is an Infected
	ASSERT_TRUE(dynamic_cast<Infected*>(pPost_TargetUnit));
	// Make sure the Unit at the target location belongs to the same player
	ASSERT_EQ(rActingPlayer.GetID(), pPost_TargetUnit->GetControllingPlayerID());
	// Make sure the Infected's cooldown has started
	ASSERT_TRUE(pUnit->GetSpecialAttackCooldown() > 0);

	pPost_TargetUnit = nullptr;
	delete pResult; pResult = nullptr;
	pOrder = nullptr;
	delete pSpecialAttackAction; pSpecialAttackAction = nullptr;
	pPre_TargetUnit = nullptr;
	pUnit = nullptr;
	delete pInfectedSpecialMapSetup; pInfectedSpecialMapSetup = nullptr;
	delete pTestInfectedSpecialMapData; pTestInfectedSpecialMapData = nullptr;
}