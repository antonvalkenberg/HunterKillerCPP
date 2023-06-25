#pragma once
#include "GameObject.h"

class MapFeature :
    public GameObject
{
public:
    MapFeature() = default;
    MapFeature(MapLocation& rMapLocation, const bool destructible, const bool blockingLOS, const bool walkable) : GameObject(rMapLocation, HunterKillerConstants::MAPFEATURE_DEFAULT_HP), IsDestructible(destructible), IsWalkable(walkable), IsBlockingLOS(blockingLOS) {}
    MapFeature(MapLocation& rMapLocation, const int maxHP, const bool destructible, const bool blockingLOS, const bool walkable) : GameObject(rMapLocation, maxHP, maxHP), IsDestructible(destructible), IsWalkable(walkable), IsBlockingLOS(blockingLOS) {}
    MapFeature(MapLocation& rMapLocation, const int maxHP, const int currentHP, const bool destructible, const bool blockingLOS, const bool walkable) : GameObject(rMapLocation, maxHP, currentHP), IsDestructible(destructible), IsWalkable(walkable), IsBlockingLOS(blockingLOS) {}
    ~MapFeature() override = default;
    bool operator==(MapFeature& rMapFeature) { return GetHashCode() == rMapFeature.GetHashCode(); }
    bool operator!=(MapFeature& rMapFeature) { return !(*this == rMapFeature); }
    bool Equals(MapFeature& rMapFeature) { return *this == rMapFeature; }
    int GetHashCode() override { return 47 ^ GetID() ^ GetLocation().GetHashCode() ^ GetCurrentHP(); }
    bool GetIsDestructible() const { return IsDestructible; }
    void SetDestructible(const bool isDestructible) { IsDestructible = isDestructible; }
    bool GetIsWalkable() const { return IsWalkable; }
    void SetWalkable(const bool isWalkable) { IsWalkable = isWalkable; }
    bool GetIsBlockingLOS() const { return IsBlockingLOS; }
    virtual TileType GetType() = 0;
private:
    bool IsDestructible = HunterKillerConstants::MAPFEATURE_DEFAULT_DESTRUCTIBLE;
    bool IsWalkable = HunterKillerConstants::MAPFEATURE_DEFAULT_WALKABLE;
protected:
    bool IsBlockingLOS = HunterKillerConstants::MAPFEATURE_DEFAULT_BLOCKING_LOS;
};

