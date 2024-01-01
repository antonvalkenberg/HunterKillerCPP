#pragma once
#include "Enums.h"
#include "MapFeature.h"

class Space :
    public MapFeature
{
public:
    Space() = default;
    Space(const Space& rSpace) : Space(rSpace.GetLocation()) { SetID(rSpace.GetID()); }
    explicit Space(MapLocation& rLocation) : MapFeature(rLocation, HunterKillerConstants::SPACE_DESTRUCTIBLE, HunterKillerConstants::SPACE_BLOCKS_LOS, HunterKillerConstants::SPACE_WALKABLE) {}
    ~Space() override = default;
    Space* Copy() override { return new Space(*this); }
    std::string ToString() override { return std::format("{}", static_cast<char>(SPACE)); }
    std::string ToStringInformational() override { return ToString(); }
    TileType GetType() override { return TileType::SPACE; }
};

