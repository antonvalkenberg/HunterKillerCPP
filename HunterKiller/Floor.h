#pragma once
#include "Enums.h"
#include "MapFeature.h"

class Floor :
    public MapFeature
{
public:
    Floor() = default;
    Floor(const Floor& rFloor) : Floor(rFloor.GetLocation()) { SetID(rFloor.GetID()); }
    explicit Floor(MapLocation& rLocation) : MapFeature(rLocation, HunterKillerConstants::FLOOR_DESTRUCTIBLE, HunterKillerConstants::FLOOR_BLOCKS_LOS, HunterKillerConstants::FLOOR_WALKABLE) {}
    ~Floor() override = default;
    Floor* Copy() override { return new Floor(*this); }
    std::string ToString() override { return std::format("{}", static_cast<char>(FLOOR)); }
    TileType GetType() override { return TileType::FLOOR; }
};

