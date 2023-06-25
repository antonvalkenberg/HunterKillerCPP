// ReSharper disable IdentifierTypo
// ReSharper disable CommentTypo
#include "pch.h"
#include "../HunterKiller/HunterKillerMap.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/GameObject.h"
#include "../HunterKiller/Door.h"
#include "../HunterKiller/Floor.h"
#include "../HunterKiller/Space.h"
#include "../HunterKiller/Structure.h"
#include "../HunterKiller/Wall.h"

class MapTest : public testing::Test
{
protected:
    void SetUp() override
    {
        TestMap = new HunterKillerMap(*TestMapName, 4, 4);
    }
    void TearDown() override
    {
        delete TestMap; TestMap = nullptr;
		delete TestMapName; TestMapName = nullptr;
    }
	const std::string* TestMapName = new std::string("map_test");
    HunterKillerMap* TestMap = nullptr;
};

class MapPathfindingTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        TestMapPathfinding = new MapSetup(*TestPathfindingMapData);
    }
    void TearDown() override
    {
        delete TestMapPathfinding; TestMapPathfinding = nullptr;
		delete TestPathfindingMapData; TestPathfindingMapData = nullptr;
    }
	const std::string* TestPathfindingMapData = new std::string("B_____\n______\n______\n______\n_|D||_\n_|__|_");
    MapSetup* TestMapPathfinding = nullptr;
};

TEST(MapLocationTest, MapLocationInitialXAndY) {
    const MapLocation* location = new MapLocation(1, 2);
    ASSERT_EQ(1, location->GetX());
    ASSERT_EQ(2, location->GetY());
    delete location;
}

TEST(MapLocationTest, MapLocationDistanceCalculations)
{
    const MapLocation* location1 = new MapLocation(0, 0);
    const MapLocation* location2 = new MapLocation(2, 1);
    ASSERT_EQ(std::sqrt(5.0), MapLocation::GetEuclideanDistance(*location1, *location2));
    ASSERT_EQ(std::sqrt(5.0), MapLocation::GetEuclideanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    ASSERT_EQ(3, MapLocation::GetManhattanDistance(*location1, *location2));
    ASSERT_EQ(3, MapLocation::GetManhattanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    delete location1; delete location2;

    location1 = new MapLocation(2, 1);
    location2 = new MapLocation(0, 0);
    ASSERT_EQ(std::sqrt(5.0), MapLocation::GetEuclideanDistance(*location1, *location2));
    ASSERT_EQ(std::sqrt(5.0), MapLocation::GetEuclideanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    ASSERT_EQ(3, MapLocation::GetManhattanDistance(*location1, *location2));
    ASSERT_EQ(3, MapLocation::GetManhattanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    delete location1; delete location2;

    location1 = new MapLocation(3, 3);
    location2 = new MapLocation(3, 3);
    ASSERT_EQ(0.0, MapLocation::GetEuclideanDistance(*location1, *location2));
    ASSERT_EQ(0.0, MapLocation::GetEuclideanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    ASSERT_EQ(0, MapLocation::GetManhattanDistance(*location1, *location2));
    ASSERT_EQ(0, MapLocation::GetManhattanDistance(location1->GetX(), location1->GetY(), location2->GetX(), location2->GetY()));
    delete location1; delete location2;
}

TEST_F(MapTest, MapLocationConversionToPosition)
{
    const int mapWidth = TestMap->GetMapWidth();

    const MapLocation* pBase = new MapLocation(0, 0);
    MapLocation& rConvertedBase = TestMap->ToLocation(0);
    ASSERT_EQ(0, HunterKillerMap::ToPosition(*pBase, mapWidth));
    ASSERT_EQ(0, HunterKillerMap::ToPosition(0, 0, mapWidth));
    ASSERT_EQ(0, rConvertedBase.GetX());
    ASSERT_EQ(0, rConvertedBase.GetY());
    delete pBase;

    pBase = new MapLocation(2, 1);
    rConvertedBase = TestMap->ToLocation(6);
    ASSERT_EQ(6, HunterKillerMap::ToPosition(*pBase, mapWidth));
    ASSERT_EQ(6, HunterKillerMap::ToPosition(2, 1, mapWidth));
    ASSERT_EQ(6, HunterKillerMap::ToPosition(pBase->GetX(), pBase->GetY(), mapWidth));
    ASSERT_EQ(2, rConvertedBase.GetX());
    ASSERT_EQ(1, rConvertedBase.GetY());
    delete pBase;
}

TEST(MapCreationTest, MapFeaturesCreation)
{
    auto* pFactory = new HunterKillerStateFactory();
	const std::string* pTestMapName = new std::string("feature_creation_test");
	const std::string* pTestMapData = new std::string("._|\nDBO\nXPH");
	auto* pSetup = new MapSetup(*pTestMapName, *pTestMapData, 3, 3, NORTH);
	const std::string* pPlayerAName = new std::string("A");
	const std::string* pPlayerBName = new std::string("B");
	auto* pPlayers = new std::vector{ new HunterKillerPlayer(0, *pPlayerAName, 0), new HunterKillerPlayer(1, *pPlayerBName, 8) };
	
    // Test the creation from a pre made map
	pFactory->ConstructInternalMap(*pSetup, *pPlayers);
    HunterKillerMap* pCreatedMap = pFactory->Map;

	// Go through the MapFeatures
	auto* pMapContent = pCreatedMap->MapContent;
	int index = HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX;

	// Map visualisation:
	// . _ | | _ .
	// D B O O _ D
	// X P H H P X
	// X P H H P X
	// D _ O O B D
	// . _ | | _ .

	// Should be 36 positions on the map
	ASSERT_EQ(36, pMapContent->size());
	
	// Go through all positions and check if the correct object was created
	ASSERT_TRUE(dynamic_cast<Space*>(pMapContent->at(0).at(index)));
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(1).at(index))); // This is also the spawn point for top-left base
	ASSERT_TRUE(dynamic_cast<Wall*>(pMapContent->at(2).at(index)));
	ASSERT_TRUE(dynamic_cast<Wall*>(pMapContent->at(3).at(index)));
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(4).at(index)));
	ASSERT_TRUE(dynamic_cast<Space*>(pMapContent->at(5).at(index)));

	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(6).at(index))); // This door should be closed
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(7).at(index)));
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(8).at(index))); // This door should be open
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(9).at(index))); // This door should be open
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(10).at(index)));
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(11).at(index))); // This door should be closed

	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(12).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(13).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(14).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(15).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(16).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(17).at(index)));

	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(18).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(19).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(20).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(21).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(22).at(index)));
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(23).at(index)));

	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(24).at(index))); // This door should be closed
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(25).at(index)));
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(26).at(index))); // This door should be open
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(27).at(index))); // This door should be open
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(28).at(index)));
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(29).at(index))); // This door should be closed

	ASSERT_TRUE(dynamic_cast<Space*>(pMapContent->at(30).at(index)));
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(31).at(index)));
	ASSERT_TRUE(dynamic_cast<Wall*>(pMapContent->at(32).at(index)));
	ASSERT_TRUE(dynamic_cast<Wall*>(pMapContent->at(33).at(index)));
	ASSERT_TRUE(dynamic_cast<Floor*>(pMapContent->at(34).at(index))); // This is also the spawn point for bottom-right base
	ASSERT_TRUE(dynamic_cast<Space*>(pMapContent->at(35).at(index)));

	// Check if the correct types of Structures are created
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(7).at(index))->GetStructureType() == STRUCTURE_BASE);

	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(12).at(index))->GetStructureType() == STRUCTURE_OBJECTIVE);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(13).at(index))->GetStructureType() == STRUCTURE_OUTPOST);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(14).at(index))->GetStructureType() == STRUCTURE_STRONGHOLD);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(15).at(index))->GetStructureType() == STRUCTURE_STRONGHOLD);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(16).at(index))->GetStructureType() == STRUCTURE_OUTPOST);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(17).at(index))->GetStructureType() == STRUCTURE_OBJECTIVE);

	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(18).at(index))->GetStructureType() == STRUCTURE_OBJECTIVE);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(19).at(index))->GetStructureType() == STRUCTURE_OUTPOST);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(20).at(index))->GetStructureType() == STRUCTURE_STRONGHOLD);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(21).at(index))->GetStructureType() == STRUCTURE_STRONGHOLD);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(22).at(index))->GetStructureType() == STRUCTURE_OUTPOST);
	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(23).at(index))->GetStructureType() == STRUCTURE_OBJECTIVE);

	ASSERT_TRUE(dynamic_cast<Structure*>(pMapContent->at(28).at(index))->GetStructureType() == STRUCTURE_BASE);

	// Check if the doors are created correctly (open/closed)
	// Closed door positions: 6, 11, 24, 29
	ASSERT_FALSE(dynamic_cast<Door*>(pMapContent->at(6).at(index))->IsOpen());
	ASSERT_FALSE(dynamic_cast<Door*>(pMapContent->at(11).at(index))->IsOpen());
	ASSERT_FALSE(dynamic_cast<Door*>(pMapContent->at(24).at(index))->IsOpen());
	ASSERT_FALSE(dynamic_cast<Door*>(pMapContent->at(29).at(index))->IsOpen());
	// Open door positions: 8, 9, 26, 27
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(8).at(index))->IsOpen());
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(9).at(index))->IsOpen());
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(26).at(index))->IsOpen());
	ASSERT_TRUE(dynamic_cast<Door*>(pMapContent->at(27).at(index))->IsOpen());

	// Check if the bases have their spawn locations set correctly
	int topLeftSpawnPosition = HunterKillerMap::ToPosition(*dynamic_cast<Structure*>(pMapContent->at(7).at(index))->GetSpawnLocation(), pCreatedMap->GetMapWidth());
	ASSERT_EQ(1, topLeftSpawnPosition);
	int bottomRightSpawnPosition = HunterKillerMap::ToPosition(*dynamic_cast<Structure*>(pMapContent->at(28).at(index))->GetSpawnLocation(), pCreatedMap->GetMapWidth());
	ASSERT_EQ(34, bottomRightSpawnPosition);
		
	delete pFactory; pFactory = nullptr;
	for (HunterKillerPlayer* pPlayer : *pPlayers)
	{
		delete pPlayer; pPlayer = nullptr;
	}
	delete pPlayers; pPlayers = nullptr;
	delete pTestMapName; pTestMapName = nullptr;
	delete pTestMapData; pTestMapData = nullptr;
	delete pPlayerAName; pPlayerAName = nullptr;
	delete pPlayerBName; pPlayerBName = nullptr;
	pMapContent = nullptr;
	pCreatedMap = nullptr;
	pSetup = nullptr;
}

TEST(FactoryTest, StateFactoryInitialState)
{
    auto* pPlayerAName = new std::string("playerA");
    auto* pPlayerBName = new std::string("playerB");
	auto* pPlayerNames = new std::vector{ pPlayerAName, pPlayerBName };
	// Create an initial state
	auto* pFactory = new HunterKillerStateFactory();
    const HunterKillerState* pInitialState = pFactory->GenerateInitialState(*pPlayerNames);

	// Check that the initialState starts in round 1
	ASSERT_EQ(1, pInitialState->GetCurrentRound());
	// Make sure the initial state is not done
	ASSERT_FALSE(pInitialState->IsDone());

	delete pInitialState; pInitialState = nullptr;
	delete pFactory; pFactory = nullptr;
	for (auto* pName : *pPlayerNames)
	{
		delete pName; pName = nullptr;
	}
	delete pPlayerNames; pPlayerNames = nullptr;
	pPlayerAName = nullptr;
	pPlayerBName = nullptr;
}

TEST_F(MapPathfindingTest, MapFindPath)
{
	auto* pPlayerAName = new std::string("playerA");
	auto* pPlayerBName = new std::string("playerB");
	auto* pPlayerNames = new std::vector{ pPlayerAName, pPlayerBName };
	// Re-create the map using the map for testing pathfinding
	auto* pFactory = new HunterKillerStateFactory();
    const HunterKillerState* pState = pFactory->GenerateInitialStateFromSetup(*pPlayerNames, *TestMapPathfinding, &HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS);
    const HunterKillerMap& rMap = pState->GetMap();

	// State of the map visualised:
	// B _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ | D | | _ _ | | D | _
	// _ | _ _ | _ _ | _ _ | _
	// _ | _ _ | _ _ | _ _ | _
	// _ | D | | _ _ | | D | _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ B

	// Test 1
	// Ask the map to calculate a path from the Player's base to the opponent's spawn location
    const HunterKillerPlayer* pPlayer0 = pState->GetPlayer(0);
	MapLocation* pBaseLocation = rMap.GetObjectLocation(pPlayer0->GetCommandCenterID());
    const HunterKillerPlayer* pPlayer1 = pState->GetPlayer(1);
	MapLocation* pP1SpawnLocation = dynamic_cast<Structure*>(rMap.GetObject(pPlayer1->GetCommandCenterID()))->GetSpawnLocation();

    const std::vector<MapLocation*>* pPath1 = rMap.FindPath(*pBaseLocation, *pP1SpawnLocation);

	// Check that the length is correct (specific path can vary)
	ASSERT_EQ(21, pPath1->size());

	// Test 2
	// Ask the map to calculate this path (start = '.', target = '*')
	// B _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ | D | | _ _ | | D | _
	// . | _ _ | _ _ | * _ | _
	// _ | _ _ | _ _ | _ _ | _
	// _ | D | | _ _ | | D | _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ _
	// _ _ _ _ _ _ _ _ _ _ _ B
	std::vector<MapLocation*>* pPath2 = rMap.FindPath(rMap.ToLocation(rMap.ToPosition(0, 5)), rMap.ToLocation(rMap.ToPosition(8, 5)));

	// Check that the length is correct
	ASSERT_EQ(14, pPath2->size());
	// Check that the path is correct
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(0, 4)), *pPath2->at(0));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(0, 3)), *pPath2->at(1));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(1, 3)), *pPath2->at(2));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(2, 3)), *pPath2->at(3));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(3, 3)), *pPath2->at(4));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(4, 3)), *pPath2->at(5));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(5, 3)), *pPath2->at(6));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(6, 3)), *pPath2->at(7));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(7, 3)), *pPath2->at(8));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(8, 3)), *pPath2->at(9));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(9, 3)), *pPath2->at(10));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(9, 4)), *pPath2->at(11));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(9, 5)), *pPath2->at(12));
	ASSERT_EQ(rMap.ToLocation(rMap.ToPosition(8, 5)), *pPath2->at(13));

	delete pPath2; pPath2 = nullptr;
	delete pPath1; pPath1 = nullptr;
	pPlayer0 = nullptr;
	pBaseLocation = nullptr;
	pPlayer1 = nullptr;
	pP1SpawnLocation = nullptr;
	delete pState; pState = nullptr;
	delete pFactory; pFactory = nullptr;
	for (auto* pName : *pPlayerNames)
	{
		delete pName; pName = nullptr;
	}
	delete pPlayerNames; pPlayerNames = nullptr;
	pPlayerAName = nullptr;
	pPlayerBName = nullptr;
}