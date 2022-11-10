#include "pch.h"
#include "../HunterKiller/HelperStructs.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/HunterKillerRules.h"

class AttackTest : public testing::Test
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
    const std::string* TestMapData = new std::string("BS");
    MapSetup* TestMapSetup = nullptr;
    HunterKillerStateFactory* Factory = nullptr;
    HunterKillerState* State = nullptr;
};

TEST_F(AttackTest, UnitDeath)
{   // Situation before attack:
	// B S _ _
	// _ _ S B
    const HunterKillerPlayer& rActivePlayer = State->GetActivePlayer();
    const int pre_ActivePlayerScore = rActivePlayer.GetScore();
    HunterKillerMap& rMap = State->GetMap();
    
    Unit* pUnit = rMap.GetUnitAtLocation(rMap.ToLocation(rMap.ToPosition(1, 0)));
    // Make sure it is facing the correct way to be attacking, because Units created on the Map at the start of a game are facing NORTH by default.
    pUnit->SetOrientation(EAST);
    // Update the Unit's field-of-view because we just changed it
    pUnit->InvalidateFieldOfView();
    pUnit->UpdateFieldOfView(rMap.GetFieldOfView(*pUnit));

    MapLocation& rTargetLocation = rMap.ToLocation(rMap.ToPosition(2, 1));
    // Set the health of the Unit at the target location so that it dies to the attack we'll be making
    Unit* pTargetUnit = rMap.GetUnitAtLocation(rTargetLocation);
    pTargetUnit->ReduceHP(pTargetUnit->GetCurrentHP() - HunterKillerConstants::SOLDIER_ATTACK_DAMAGE);

    // Create an order to attack the target location
    auto* pAttackAction = new HunterKillerAction(*State);
    auto* pAttackOrder = new UnitOrder(pUnit->GetID(), ATTACK, UNIT_SOLDIER, rTargetLocation);
    pAttackAction->TryAddOrder(pAttackOrder);

    // Make the game logic execute the action
    const Result* pResult = HunterKillerRules::Handle(*State, *pAttackAction);

    // Make sure the order did not fail
    ASSERT_TRUE(pResult->Accepted);
    ASSERT_TRUE(pResult->Information->empty());

    // Make sure there is no Unit anymore on the target location
    ASSERT_FALSE(rMap.GetUnitAtLocation(rTargetLocation));

    // Make sure the player was awarded the correct score amount
    const int post_ActivePlayerScore = rActivePlayer.GetScore();
    ASSERT_EQ(HunterKillerConstants::SOLDIER_SCORE, post_ActivePlayerScore - pre_ActivePlayerScore);

    delete pResult; pResult = nullptr;
    delete pAttackAction; pAttackAction = nullptr;
}