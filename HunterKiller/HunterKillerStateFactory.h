#pragma once
#include "HunterKillerState.h"
#include "packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.6/build/native/include/gtest/gtest_prod.h"

class HunterKillerStateFactory
{
public:
    HunterKillerStateFactory();
    ~HunterKillerStateFactory();
    HunterKillerState* GenerateInitialState(const std::vector<std::string*>& rPlayerNames, const std::string* pOptions = nullptr);
    HunterKillerState* GenerateInitialStateFromSetup(const std::vector<std::string*>& rPlayerNames, MapSetup& rMapSetup, const std::string* pOptions = nullptr);
    void CreateGameObjectInMap(char data, int x, int y, FourPatchSections section, Direction baseSpawnDirection) const;
    void SetupMap(int width, int height);
private:
    HunterKillerMap* Map = nullptr;
    MapSetup* MapSettings = nullptr;
    std::map<int, int>* MapSectionPlayerIDMap = nullptr;
    std::vector<MapSetup*>* MapRotation = nullptr;
    static std::map<int, int>* CreateMapSectionPlayerIDMapping(const std::vector<HunterKillerPlayer*>& rPlayers);
    void ConstructInternalMap(MapSetup& rMapSetup, const std::vector<HunterKillerPlayer*>& rPlayers);
    void MirrorQuadrantsIntoMap();
    void Reset();
    FRIEND_TEST(MapCreationTest, MapFeaturesCreation);
    FRIEND_TEST(MapPathfindingTest, MapFindPath);
};

