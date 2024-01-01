// ReSharper disable IdentifierTypo
#pragma once
#include <string>

#include "Enums.h"
#include "IControlled.h"
#include "MapFeature.h"

class Structure :
    public MapFeature, public IControlled
{
public:
    Structure() = default;
    Structure(MapLocation& rLocation, StructureType type);
    Structure(const Structure& rStructure);
    ~Structure() override;
    Structure* Copy() override;
    std::string ToString() override;
    std::string ToStringInformational() override;
    StructureType GetStructureType() const { return TypeOfStructure; }
    TileType GetType() override { return BASE; }
    bool IsControlledBy(const int playerID) const override { return ControllingPlayerID == playerID; }
    int GetControllingPlayerID() const override { return ControllingPlayerID; }
    void SetControllingPlayerID(const int playerID) { ControllingPlayerID = playerID; }
    bool IsUnderControl() const { return ControllingPlayerID != HunterKillerConstants::STRUCTURE_NO_CONTROL; }
    bool GetIsCapturable() const { return IsCapturable; }
    bool GetAllowsSpawning() const { return AllowsSpawning; }
    [[nodiscard]] MapLocation* GetSpawnLocation() const { return SpawnLocation; }
    void SetSpawnLocation(MapLocation& rLocation) { SpawnLocation = &rLocation; }
    bool GetGeneratesResource() const { return GeneratesResource; }
    int GetResourceGeneration() const { return ResourceGeneration; }
    bool GetGeneratesScore() const { return GeneratesScore; }
    int GetScoreGeneration() const { return ScoreGeneration; }
    bool GetIsCommandCenter() const { return IsCommandCenter; }
private:
    StructureType TypeOfStructure = STRUCTURE_BASE;
    int ControllingPlayerID = HunterKillerConstants::STRUCTURE_NO_CONTROL;
    bool IsCapturable = HunterKillerConstants::STRUCTURE_CAPTURABLE;
    bool AllowsSpawning = HunterKillerConstants::STRUCTURE_ALLOW_SPAWNING;
    MapLocation* SpawnLocation = nullptr;
    bool GeneratesResource = HunterKillerConstants::STRUCTURE_GENERATES_RESOURCE;
    int ResourceGeneration = HunterKillerConstants::STRUCTURE_RESOURCE_GENERATION;
    bool GeneratesScore = HunterKillerConstants::STRUCTURE_GENERATES_SCORE;
    int ScoreGeneration = HunterKillerConstants::STRUCTURE_SCORE_GENERATION;
    bool IsCommandCenter = HunterKillerConstants::STRUCTURE_IS_COMMAND_CENTER;
};

