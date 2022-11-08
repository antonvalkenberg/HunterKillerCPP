// ReSharper disable CommentTypo
#pragma once
#include "HunterKillerConstants.h"
#include "MapLocation.h"
#include "Node.h"
#include "StringHelpFunctions.h"

struct MapSetup
{
    const std::string& Name;
    const std::string& MapData;
    bool Custom = false;
    int QuadrantAWidth = -1;
    int QuadrantAHeight = -1;
    Direction SpawnDirection = HunterKillerConstants::BASE_DEFAULT_SPAWN_DIRECTION;
    int StartingResources = HunterKillerConstants::PLAYER_STARTING_RESOURCE;
    int BaseResourceGeneration = HunterKillerConstants::STRUCTURE_RESOURCE_GENERATION;
    explicit MapSetup(const std::string& mapData)
        : Name(HunterKillerConstants::DEFAULT_MAP_NAME), MapData(mapData)
    {
        SetQuadrantADimensionsFromData();
    }
    MapSetup(const std::string& name, const std::string& mapData, const bool custom)
        : Name(name), MapData(mapData), Custom(custom)
    {
        // For custom maps, the quadrant height/width will be calculated during map construction
        if (!Custom)
            SetQuadrantADimensionsFromData();
    }
    MapSetup(const std::string& name, const std::string& mapData, const int quadrantAWidth, const int quadrantAHeight, const Direction spawnDirection)
        : Name(name), MapData(mapData), QuadrantAWidth(quadrantAWidth), QuadrantAHeight(quadrantAHeight), SpawnDirection(spawnDirection) {}
    MapSetup(const std::string& name, const std::string& mapData, const int quadrantAWidth, const int quadrantAHeight, const Direction spawnDirection, const int startingResources, const int baseResourceGeneration)
        : Name(name), MapData(mapData), QuadrantAWidth(quadrantAWidth), QuadrantAHeight(quadrantAHeight), SpawnDirection(spawnDirection), StartingResources(startingResources), BaseResourceGeneration(baseResourceGeneration) {}
private:
    void SetQuadrantADimensionsFromData()
    {
        const std::vector<std::string>* lines = split_string(MapData, HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
        QuadrantAWidth = static_cast<int>(lines->at(0).length());
        QuadrantAHeight = static_cast<int>(lines->size());
        delete lines;
        lines = nullptr;
    }
};

struct OrderStatistics {
    int Issued = 0;
    int Success = 0;
    int Failed = 0;
    int SpawnSoldier = 0;
    int SpawnMedic = 0;
    int SpawnInfected = 0;
    int Move = 0;
    int RotateClockwise = 0;
    int RotateCounter = 0;
    int Attack = 0;
    int Heal = 0;
    int Grenade = 0;
    int AttackUnit = 0;
    int AttackStructure = 0;
    int AttackAlly = 0;
    std::string ToString() {
        return std::format("OrderStatistics: {0},{1},{2} | {3},{4},{5} | {6},{7},{8} | {9},{10},{11} | {12},{13},{14}",
            Issued, Success, Failed,
            SpawnSoldier, SpawnMedic, SpawnInfected,
            Move, RotateClockwise, RotateCounter,
            Attack, Heal, Grenade,
            AttackUnit, AttackStructure, AttackAlly);
    }
};

struct LineOfSight_CacheEntry {
    MapLocation& Location;
    int Range;
    Direction SomeDirection;
    float AngleLimit;
    LineOfSight_CacheEntry(MapLocation& rLocation, const int range, const Direction direction, const float angleLimit) : Location(rLocation), Range(range), SomeDirection(direction), AngleLimit(angleLimit) {}
    bool operator==(const LineOfSight_CacheEntry& rCacheEntry) const { return this->Equals(rCacheEntry); }
    bool operator!=(const LineOfSight_CacheEntry& rCacheEntry) const { return !this->Equals(rCacheEntry); }
    bool Equals(const LineOfSight_CacheEntry& rCacheEntry) const { return this->GetHashCode() == rCacheEntry.GetHashCode(); }
    int GetHashCode() const { return 47 ^ Location.GetHashCode() ^ Range ^ static_cast<int>(SomeDirection) ^ static_cast<int>(AngleLimit); }
};

struct LineOfSight_Slope {
    int Y;
    int X;
    LineOfSight_Slope(const int y, const int x) : Y(y), X(x) {}
    bool Greater(const int y, const int x) const { return Y * x > X * y; }
    bool GreaterOrEqual(const int y, const int x) const { return Y * x >= X * y; }
    bool Less(const int y, const int x) const { return Y * x < X* y; }
    bool LessOrEqual(const int y, const int x) const { return Y * x <= X * y; }
};

class IntPayload {
public:
    IntPayload() = default;
    explicit IntPayload(const int value) : Value(value) {}
    ~IntPayload() = default;
    bool operator>(const IntPayload& rOther) const { return this->Value > rOther.Value; }
    int GetHashCode() const { return Value; }
    int GetValue() const { return Value; }
private:
    int Value = 0;
};

struct HunterKillerMap_PathNode : Node<IntPayload> {
    int Position = -1;
    IntPayload* PathCost = nullptr;
    int EstimatedCostToTarget = -1;
    bool Closed = false;
    explicit HunterKillerMap_PathNode(IntPayload* value) : Node<IntPayload>(value), PathCost(value) {}
    HunterKillerMap_PathNode(IntPayload* value, HunterKillerMap_PathNode* pParent) : Node<IntPayload>(value, pParent), PathCost(value) {}
    ~HunterKillerMap_PathNode() { delete PathCost; PathCost = nullptr; }
    void SetPathCost(IntPayload* pPathCost) { delete PathCost; PathCost = pPathCost; SetPayload(pPathCost); }
};