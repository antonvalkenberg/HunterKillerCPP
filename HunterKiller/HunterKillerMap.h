// ReSharper disable IdentifierTypo
#pragma once
#include <map>
#include <ranges>
#include <string>

#include "Door.h"
#include "GameObject.h"
#include "HelperStructs.h"
#include "LineOfSight.h"
#include "MapFeature.h"
#include "Structure.h"
#include "Unit.h"
#include "UnitOrder.h"
#include "packages/Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn.1.8.1.6/build/native/include/gtest/gtest_prod.h"

class HunterKillerMap
{
    friend class HunterKillerRules;
public:
    explicit HunterKillerMap(const std::string& name) : Name(name) {}
    HunterKillerMap(const HunterKillerMap& rMap);
    HunterKillerMap(const std::string& name, int width, int height);
    ~HunterKillerMap();
    [[nodiscard]] HunterKillerMap* Copy() const { return new HunterKillerMap(*this); }
    int GetHashCode() const;
    int ToPosition(const MapLocation& rLocation) const { return ToPosition(rLocation, MapWidth); }
    static int ToPosition(const MapLocation& rLocation, const int width) { return ToPosition(rLocation.GetX(), rLocation.GetY(), width); }
    int ToPosition(const int x, const int y) const { return ToPosition(x, y, MapWidth); }
    static int ToPosition(const int x, const int y, const int width) { return y * width + x; }
    [[nodiscard]] MapLocation& ToLocation(const int position) const { return *Locations->at(position); }
    [[nodiscard]] MapLocation& ToLocation(const int x, const int y) const { return ToLocation(ToPosition(x, y)); }
    bool IsXOnMap(const int x) const { return x >= 0 && x < MapWidth; }
    bool IsYOnMap(const int y) const { return y >= 0 && y < MapHeight; }
    bool IsOnMap(const MapLocation& rLocation) const { return IsXOnMap(rLocation.GetX()) && IsYOnMap(rLocation.GetY()); }
    bool IsTraversable(const MapLocation& rLocation, std::string* pFailureReasons) const;
    bool IsTraversable(const MapLocation& rLocation) const { return IsTraversable(rLocation, nullptr); }
    bool IsMovePossible(const MapLocation& rFromLocation, const UnitOrder& rMove, std::string* pFailureReasons) const;
    bool IsMovePossible(const MapLocation& rFromLocation, Direction direction) const;
    bool Move(const MapLocation& rTargetLocation, GameObject& rGameObject, std::string* pFailureReasons);
    [[nodiscard]] MapLocation* GetLocationInDirection(const MapLocation& rFromLocation, Direction direction, int distance) const;
    [[nodiscard]] MapLocation* GetAdjacentLocationInDirection(const MapLocation& rLocation, const Direction direction) const { return GetLocationInDirection(rLocation, direction, 1); }
    int GetPositionInDirection(int position, Direction direction, int distance) const;
    int GetMaxTravelDistance(const MapLocation& rLocation, Direction direction) const;
    void GetNeighbours(const MapLocation& rLocation, std::unordered_set<MapLocation, MapLocationHash>& rNeighbourCollection) const;
    void GetAreaAround(MapLocation& rLocation, bool includeCentre, std::unordered_set<MapLocation, MapLocationHash>& rAreaCollection) const;
    void GetMapFeaturesAround(const MapLocation& rLocation, std::vector<std::vector<MapFeature*>>& rAreaCollection) const;
    [[nodiscard]] std::unordered_set<MapLocation, MapLocationHash>* GetFieldOfView(const Unit& rUnit);
    [[nodiscard]] std::unordered_set<MapLocation, MapLocationHash>* GetFieldOfView(const Structure& rStructure) const;
    [[nodiscard]] GameObject* GetObject(const int objectID) const { if (!Objects->contains(objectID)) return nullptr; return Objects->at(objectID); }
    [[nodiscard]] std::unordered_map<int, GameObject*>* GetObjects() const { return Objects; }
    [[nodiscard]] MapLocation* GetObjectLocation(int objectID) const;
    [[nodiscard]] Unit* GetUnitAtLocation(const MapLocation& rLocation) const;
    [[nodiscard]] MapFeature* GetFeatureAtLocation(const MapLocation& rLocation) const;
    int GetCurrentCommandCenterCount() const { if (!CommandCenterObjectIDs) return 0; return static_cast<int>(CommandCenterObjectIDs->size()); }
    bool IsAttackOrderWithoutTarget(const UnitOrder& rOrder) const;
    bool IsAttackOrderTargetingAllyStructure(const UnitOrder& rOrder, const Unit* pUnit) const;
    bool IsAttackOrderTargetingAllyUnit(const UnitOrder& rOrder, const Unit* pUnit) const;
    void RegisterGameObject(GameObject* pGameObject) const;
    void UnregisterGameObject(GameObject* pGameObject) const;
    void UpdateFieldOfView();
    void InvalidateFieldOfViewFor(const MapLocation& rLocation);
    bool Place(const MapLocation& rLocation, GameObject* pObject) const { return Place(ToPosition(rLocation), pObject); }
    bool Place(int position, GameObject* pObject) const;
    bool Remove(const MapLocation& rLocation, GameObject* pObject) const { return Remove(ToPosition(rLocation), pObject); }
    bool Remove(int position, GameObject* pObject) const;
    [[nodiscard]] std::vector<MapLocation*>* FindPath(const MapLocation& rFrom, const MapLocation& rTo) const;
    std::string ToString() const;
    void Timer();
    bool TryCloseDoor(Door& rDoor) const;
    bool BlocksLight(int x, int y) const;
    // Distance of x,y coordinates relative to 0,0. Used in Line-of-Sight calculations.
    int GetManhattanDistance(const int x, const int y) const { return MapLocation::GetManhattanDistance(0, 0, x, y); }
    // Distance of x,y coordinates relative to 0,0. Used in Line-of-Sight calculations.
    double GetEuclideanDistance(const int x, const int y) const { return MapLocation::GetEuclideanDistance(0, 0, x, y); }
    const std::string& GetName() const { return Name; }
    int GetMapHeight() const { return MapHeight; }
    int GetMapWidth() const { return MapWidth; }
    [[nodiscard]] std::vector<int>* GetIDBuffer() const { return IDBuffer; }
    [[nodiscard]] std::vector<int>* GetCommandCenterObjectIDs() const { return CommandCenterObjectIDs; }
private:
    const std::string& Name;
    int MapHeight = 0;
    int MapWidth = 0;
    std::vector<std::vector<GameObject*>>* MapContent = nullptr;
    std::unordered_map<int, GameObject*>* Objects = nullptr;
    std::vector<MapLocation*>* Locations = nullptr;
    std::vector<int>* IDBuffer = nullptr;
    std::vector<int>* CommandCenterObjectIDs = nullptr;
    LineOfSight* LoS = nullptr;
    void AddNode(std::unordered_map<int, HunterKillerMap_PathNode*>& rNodes, std::multimap<int, HunterKillerMap_PathNode*>& rOpen, HunterKillerMap_PathNode& rParent, const MapLocation& rLocation, int cost, const MapLocation& rTarget) const;
    FRIEND_TEST(MapCreationTest, MapFeaturesCreation);
    FRIEND_TEST(LineOfSightTest, TestOpenVision);
    FRIEND_TEST(LineOfSightTest, TestWallVision);
    FRIEND_TEST(LineOfSightTest, TestCornerVision);
    FRIEND_TEST(LineOfSightTest, TestRoomVision);
    FRIEND_TEST(LineOfSightTest, TestDoorVision);
protected:
    void CopyMapContent(std::vector<std::vector<GameObject*>>* pCopyTo, std::unordered_map<int, GameObject*>* pCopiedObjects) const;
    void SetMapContent(std::vector<std::vector<GameObject*>>* pNewMapContent) { delete MapContent; MapContent = pNewMapContent; }
    bool AttackLocation(const MapLocation& rLocation, int damage) const;
};
