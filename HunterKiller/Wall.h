#pragma once
#include "Enums.h"
#include "MapFeature.h"

class Wall :
    public MapFeature
{
public:
    Wall() = default;
    Wall(const Wall& rWall) : Wall(rWall.GetLocation()) { SetID(rWall.GetID()); }
    explicit Wall(MapLocation& rLocation) : MapFeature(rLocation, HunterKillerConstants::WALL_DESTRUCTIBLE, HunterKillerConstants::WALL_BLOCKS_LOS, HunterKillerConstants::WALL_WALKABLE) {}
    ~Wall() override = default;
    Wall* Copy() override { return new Wall(*this); }
    std::string ToString() override { return std::format("{}", static_cast<char>(WALL)); }
    std::string ToStringInformational() override { return ToString(); }
    TileType GetType() override { return TileType::WALL; }
};

