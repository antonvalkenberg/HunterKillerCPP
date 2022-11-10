#include "pch.h"
#include "../HunterKiller/HelperStructs.h"
#include "../HunterKiller/HunterKillerStateFactory.h"
#include "../HunterKiller/HunterKillerRules.h"
#include "../HunterKiller/LineOfSight.h"
#include "../HunterKiller/Floor.h"
#include "../HunterKiller/Wall.h"
#include "../HunterKiller/Door.h"
#include "../HunterKiller/Soldier.h"
#include "../HunterKiller/Infected.h"

/**
 * Tests a line-of-sight setup with no obstacles:
 * - - - -
 * - U - -
 * - - - -
 * - - - -
 *
 * Note that in this diagram, 'U' stands for the Unit and '-' stands for Floor tiles.
 * This setup is populated with MapFeatures to test different situations.
 */
class LineOfSightTest : public testing::Test
{
protected:
    void SetUp() override
    {
        TestMap = new HunterKillerMap("LoS_test", 4, 4);
    }
    void TearDown() override
    {
        delete TestMap; TestMap = nullptr;
    }
    HunterKillerMap* TestMap = nullptr;
};

TEST_F(LineOfSightTest, TestOpenVision)
{
    // Fill map with Floors
    auto* pMapContent = new std::vector<std::vector<GameObject*>>();
    pMapContent->resize(16);
    for (int i = 0; i < 16; ++i)
    {
        pMapContent->at(i).resize(2);
        Floor* pFloor = new Floor(TestMap->ToLocation(i));
        TestMap->RegisterGameObject(pFloor);
        pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pFloor;
    }
    TestMap->SetMapContent(pMapContent);

    // In the next sections, when visualizing the FOV; '.' refers to visible tiles, and '#' refers to obscured tiles.

    // Create a new soldier at [1,1] facing NORTH
    Soldier* pSoldier = new Soldier(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pSoldier);
    TestMap->Place(TestMap->ToLocation(1, 1), pSoldier);
    auto* pNorthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // . . . #
    // # U # #
    // # # # #
    // # # # #
    ASSERT_EQ(4, pNorthFoV->size());
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(1, 1)));
    delete pNorthFoV; pNorthFoV = nullptr;

    // Face the soldier east
    pSoldier->SetOrientation(EAST);
    // Get the field-of-view for the east-facing soldier
    auto* pEastFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # . .
    // # U . .
    // # # . .
    // # # # #
    ASSERT_EQ(7, pEastFoV->size());
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(3, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(3, 2)));
    delete pEastFoV; pEastFoV = nullptr;
    
    // Face the soldier south
    pSoldier->SetOrientation(SOUTH);
    auto* pSouthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # U # #
    // . . . #
    // . . . #
    ASSERT_EQ(7, pSouthFoV->size());
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 3)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(2, 3)));
    delete pSouthFoV; pSouthFoV = nullptr;

    // Face the soldier west
    pSoldier->SetOrientation(WEST);
    auto* pWestFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // . # # #
    // . U # #
    // . # # #
    // # # # #
    ASSERT_EQ(4, pWestFoV->size());
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 2)));
    delete pWestFoV; pWestFoV = nullptr;

    // We are done with our trusty soldier, so remove it
    TestMap->UnregisterGameObject(pSoldier);
    delete pSoldier; pSoldier = nullptr;

    // Create a new infected at [1,2]
    Infected* pInfected = new Infected(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pInfected);
    TestMap->Place(TestMap->ToLocation(1, 2), pInfected);
    // Get the field-of-view for the infected
    auto* pInfectedFoV = TestMap->GetFieldOfView(*pInfected);
    // Check that the FOV looks like:
    // . . . .
    // . . . .
    // . U . .
    // . . . .
    ASSERT_EQ(16, pInfectedFoV->size());
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 3)));
    delete pInfectedFoV; pInfectedFoV = nullptr;
}

/**
 * Tests a line-of-sight setup where a Unit is against a wall:
 *
 * <pre>
 *    - - [ -
 *    - U [ -
 *    - - [ -
 *    - - [ -
 * </pre>
 *
 * Note that in this diagram, 'U' stands for the Unit, '[' stands for a Wall tile, and '-' stands
 * for a Floor tile. This setup is tested with different Unit orientations and once for an
 * Infected.
 */
TEST_F(LineOfSightTest, TestWallVision)
{
    // Fill map with Floors, except for the third column
    auto* pMapContent = new std::vector<std::vector<GameObject*>>();
    pMapContent->resize(16);
    for (int i = 0; i < 16; ++i)
    {
        pMapContent->at(i).resize(2);
        MapLocation& rLocation = TestMap->ToLocation(i);
        if (rLocation.GetX() == 2)
        {
            Wall* pWall = new Wall(rLocation);
            TestMap->RegisterGameObject(pWall);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pWall;
        }
        else {
            Floor* pFloor = new Floor(rLocation);
            TestMap->RegisterGameObject(pFloor);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pFloor;
        }
    }
    TestMap->SetMapContent(pMapContent);

    // In the next sections, when visualizing the FOV; '.' refers to visible tiles, and '#' refers to obscured tiles.

    // Create a new soldier at [1,1] facing NORTH
    Soldier* pSoldier = new Soldier(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pSoldier);
    TestMap->Place(TestMap->ToLocation(1, 1), pSoldier);
    auto* pNorthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // . . . #
    // # U # #
    // # # # #
    // # # # #
    ASSERT_EQ(4, pNorthFoV->size());
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(1, 1)));
    delete pNorthFoV; pNorthFoV = nullptr;

    // Face the soldier east
    pSoldier->SetOrientation(EAST);
    // Get the field-of-view for the east-facing soldier
    auto* pEastFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # . #
    // # U . #
    // # # . #
    // # # # #
    ASSERT_EQ(4, pEastFoV->size());
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 2)));
    delete pEastFoV; pEastFoV = nullptr;

    // Face the soldier south
    pSoldier->SetOrientation(SOUTH);
    // Get the field-of-view for the south-facing soldier
    auto* pSouthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # U # #
    // . . . #
    // . . . #
    ASSERT_EQ(7, pSouthFoV->size());
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 3)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(2, 3)));
    delete pSouthFoV; pSouthFoV = nullptr;

    // Face the soldier west
    pSoldier->SetOrientation(WEST);
    // Get the field-of-view for the west-facing soldier
    auto* pWestFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // . # # #
    // . U # #
    // . # # #
    // # # # #
    ASSERT_EQ(4, pWestFoV->size());
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(0, 2)));
    delete pWestFoV; pWestFoV = nullptr;

    // We are done with our trusty soldier, so remove it
    TestMap->UnregisterGameObject(pSoldier);
    delete pSoldier; pSoldier = nullptr;

    // Create a new infected at [1,2]
    Infected* pInfected = new Infected(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pInfected);
    TestMap->Place(TestMap->ToLocation(1, 2), pInfected);
    // Get the field-of-view for the infected
    auto* pInfectedFoV = TestMap->GetFieldOfView(*pInfected);
    // Check that the FOV looks like:
    // . . . #
    // . . . #
    // . U . #
    // . . . #
    ASSERT_EQ(12, pInfectedFoV->size());
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 3)));
    delete pInfectedFoV; pInfectedFoV = nullptr;
}

/**
 * Tests a line-of-sight setup where a Unit is against a wall:
 *
 * <pre>
 *    U - - -
 *    [ - - -
 *    - - - -
 *    - - - -
 * </pre>
 *
 * Note that in this diagram, 'U' stands for the Unit, '[' stands for a Wall tile, and '-' stands
 * for a Floor tile. This setup is tested with the Unit in CLOCKWISE and SOUTH orientation and once for
 * an Infected.
 */
TEST_F(LineOfSightTest, TestCornerVision)
{
    // Fill map with Floors, except for position #4
    auto* pMapContent = new std::vector<std::vector<GameObject*>>();
    pMapContent->resize(16);
    for (int i = 0; i < 16; ++i)
    {
        pMapContent->at(i).resize(2);
        MapLocation& rLocation = TestMap->ToLocation(i);
        if (i == 4)
        {
            Wall* pWall = new Wall(rLocation);
            TestMap->RegisterGameObject(pWall);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pWall;
        }
        else {
            Floor* pFloor = new Floor(rLocation);
            TestMap->RegisterGameObject(pFloor);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pFloor;
        }
    }
    TestMap->SetMapContent(pMapContent);
    
    // In the next sections, when visualizing the FOV; '.' refers to visible tiles, and '#' refers to obscured tiles.

    // Create a new soldier at [0,0] facing EAST
    Soldier* pSoldier = new Soldier(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, EAST);
    TestMap->RegisterGameObject(pSoldier);
    TestMap->Place(TestMap->ToLocation(0,0), pSoldier);

    // Get the field-of-view for the east-facing soldier
    auto* pEastFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // U . . .
    // # . . #
    // # # # #
    // # # # #
    ASSERT_EQ(6, pEastFoV->size());
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(3, 0)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pEastFoV->contains(TestMap->ToLocation(2, 1)));
    delete pEastFoV; pEastFoV = nullptr;

    // Face the soldier south
    pSoldier->SetOrientation(SOUTH);
    // Get the field-of-view for the south-facing soldier
    auto* pSouthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // U # # #
    // . . # #
    // # . # #
    // # # # #
    ASSERT_EQ(4, pSouthFoV->size());
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pSouthFoV->contains(TestMap->ToLocation(1, 2)));

    // We are done with our trusty soldier, so remove it
    TestMap->UnregisterGameObject(pSoldier);
    delete pSoldier; pSoldier = nullptr;

    // Create a new infected at [0,0]
    Infected* pInfected = new Infected(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pInfected);
    TestMap->Place(TestMap->ToLocation(0,0), pInfected);
    // Get the field-of-view for the infected
    auto* pInfectedFoV = TestMap->GetFieldOfView(*pInfected);
    // Check that the FOV looks like:
    // U . . .
    // . . . .
    // # . . #
    // # # # #
    ASSERT_EQ(10, pInfectedFoV->size());
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 0)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(0, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 2)));
    delete pInfectedFoV; pInfectedFoV = nullptr;
}

/**
 * Tests a line-of-sight setup where a Unit is inside a room:
 *
 * <pre>
 *    - - - -
 *    - [ [ [
 *    - [ - -
 *    - [ - U
 * </pre>
 *
 * Note that in this diagram, 'U' stands for the Unit, '[' stands for a Wall tile, and '-' stands
 * for a Floor tile. This setup is tested with the Unit in NORTH and COUNTER_CLOCKWISE orientation and once for
 * an Infected.
 */
TEST_F(LineOfSightTest, TestRoomVision) {
    // Fill map with Floors, except for positions #5, 6, 7, 9, 13
    auto* pMapContent = new std::vector<std::vector<GameObject*>>();
    pMapContent->resize(16);
    for (int i = 0; i < 16; ++i)
    {
        pMapContent->at(i).resize(2);
        MapLocation& rLocation = TestMap->ToLocation(i);
        if (i == 5 || i == 6 || i == 7 || i == 9 || i == 13)
        {
            Wall* pWall = new Wall(rLocation);
            TestMap->RegisterGameObject(pWall);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pWall;
        }
        else {
            Floor* pFloor = new Floor(rLocation);
            TestMap->RegisterGameObject(pFloor);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pFloor;
        }
    }
    TestMap->SetMapContent(pMapContent);

    // In the next sections, when visualizing the FOV; '.' refers to visible tiles, and '#' refers to obscured tiles.

    // Create a new soldier at [3,3] facing WEST
    Soldier* pSoldier = new Soldier(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, WEST);
    TestMap->RegisterGameObject(pSoldier);
    TestMap->Place(TestMap->ToLocation(3, 3), pSoldier);

    // Get the field-of-view for the west-facing soldier
    auto* pWestFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # # # #
    // # . . #
    // # . . U
    ASSERT_EQ(5, pWestFoV->size());
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(3, 3)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(2, 3)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pWestFoV->contains(TestMap->ToLocation(2, 2)));
    delete pWestFoV; pWestFoV = nullptr;

    // Face the soldier north
    pSoldier->SetOrientation(NORTH);
    // Get the field-of-view for the north-facing soldier
    auto* pNorthFoV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # # . .
    // # # . .
    // # # # U
    ASSERT_EQ(5, pNorthFoV->size());
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(3, 3)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pNorthFoV->contains(TestMap->ToLocation(2, 1)));
    delete pNorthFoV; pNorthFoV = nullptr;

    // We are done with our trusty soldier, so remove it
    TestMap->UnregisterGameObject(pSoldier);
    delete pSoldier; pSoldier = nullptr;

    // Create a new infected at [3,3]
    Infected* pInfected = new Infected(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pInfected);
    TestMap->Place(TestMap->ToLocation(3, 3), pInfected);
    // Get the field-of-view for the infected
    auto* pInfectedFoV = TestMap->GetFieldOfView(*pInfected);
    // Check that the FOV looks like:
    // # # # #
    // # . . .
    // # . . .
    // # . . U
    ASSERT_EQ(9, pInfectedFoV->size());
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 3)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pInfectedFoV->contains(TestMap->ToLocation(1, 1)));
    delete pInfectedFoV; pInfectedFoV = nullptr;
}

/**
 * Tests a line-of-sight setup where a Unit is inside a room facing a door:
 *
 * <pre>
 *    - - - -
 *    - [ D [
 *    - [ - -
 *    - [ U -
 * </pre>
 *
 * Note that in this diagram, 'U' stands for the Unit, '[' stands for a Wall tile, 'D' stands for a Door tile, and
 * '-' stands for a Floor tile. This setup is tested with the Unit facing NORTH while the Door starts closed, opens
 * and is then closed again.
 */
TEST_F(LineOfSightTest, TestDoorVision) {
    // Fill map with Floors, except for positions #5, 7, 9, 13 (which are Walls), and 6 (which is a Door).
    auto* pMapContent = new std::vector<std::vector<GameObject*>>();
    pMapContent->resize(16);
    for (int i = 0; i < 16; ++i)
    {
        pMapContent->at(i).resize(2);
        MapLocation& rLocation = TestMap->ToLocation(i);
        if (i == 5 || i == 7 || i == 9 || i == 13)
        {
            Wall* pWall = new Wall(rLocation);
            TestMap->RegisterGameObject(pWall);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pWall;
        }
        else if (i == 6)
        {
            Door* pDoor = new Door(rLocation);
            TestMap->RegisterGameObject(pDoor);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pDoor;
        }
        else {
            Floor* pFloor = new Floor(rLocation);
            TestMap->RegisterGameObject(pFloor);
            pMapContent->at(i)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] = pFloor;
        }
    }
    TestMap->SetMapContent(pMapContent);
    
    // In the next sections, when visualizing the FOV; '.' refers to visible tiles, and '#' refers to obscured tiles.

    // Create a new soldier at [2,3] facing NORTH
    Soldier* pSoldier = new Soldier(0, *HunterKillerConstants::GAMEOBJECT_NOT_PLACED, NORTH);
    TestMap->RegisterGameObject(pSoldier);
    TestMap->Place(TestMap->ToLocation(2, 3), pSoldier);

    // Get the field-of-view for the north-facing soldier
    auto* pNorthFOV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # . . .
    // # . . .
    // # # U #
    ASSERT_EQ(7, pNorthFOV->size());
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 3)));
    delete pNorthFOV; pNorthFOV = nullptr;

    // Open the Door
    Door* pDoor = dynamic_cast<Door*>(TestMap->GetFeatureAtLocation(TestMap->ToLocation(2, 1)));
    pDoor->Open();

    // Update the soldier's field-of-view
    pNorthFOV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # . #
    // # . . .
    // # . . .
    // # # U #
    ASSERT_EQ(8, pNorthFOV->size());
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 0)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 3)));
    delete pNorthFOV; pNorthFOV = nullptr;

    // Fast-forward the map to close the door
    do {
        TestMap->Timer();
    } while (pDoor->IsOpen());

    // Update the soldier's field-of-view
    pNorthFOV = TestMap->GetFieldOfView(*pSoldier);
    // Check that the FOV looks like:
    // # # # #
    // # . . .
    // # . . .
    // # # U #
    ASSERT_EQ(7, pNorthFOV->size());
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 1)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(1, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(3, 2)));
    ASSERT_TRUE(pNorthFOV->contains(TestMap->ToLocation(2, 3)));
    delete pNorthFOV; pNorthFOV = nullptr;
}