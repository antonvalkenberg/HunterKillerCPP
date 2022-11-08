#pragma once
#include <unordered_set>

#include "HelperStructs.h"
#include "HunterKillerMap.h"
#include "MapLocation.h"
#include "Unit.h"

class HunterKillerPlayer
{
    friend class HunterKillerRules;
public:
    explicit HunterKillerPlayer(const std::string& name) : Name(name) {}
    HunterKillerPlayer(const int id, const std::string& name, const int mapSection) : HunterKillerPlayer(id, name, mapSection, HunterKillerConstants::PLAYER_STARTING_RESOURCE) {}
    HunterKillerPlayer(const int id, const std::string& name, const int mapSection, const int startingResources) : Name(name), ID(id), MapSection(mapSection), Resource(startingResources) {}
    ~HunterKillerPlayer();
    int GetHashCode() const { return ID; }
    std::string ToString() { return std::format("{0:s} (ID: {1:d})", Name, ID); }
    [[nodiscard]] HunterKillerPlayer* Copy() const;
    [[nodiscard]] std::unordered_set<MapLocation, MapLocationHash>* GetCombinedFieldOfView(HunterKillerMap& rMap) const;
    [[nodiscard]] std::vector<Structure*>* GetStructures(const HunterKillerMap& rMap) const;
    [[nodiscard]] std::vector<Unit*>* GetUnits(const HunterKillerMap& rMap) const;
    void AwardScore(const int value) { Score += value; }
    void AwardResource(const int amount) { Resource += amount; }
    void AssignCommandCenter(const Structure& rCommandCenter) { CommandCenterID = rCommandCenter.GetID(); }
    void AddUnit(const int unitID) const { UnitIDs->push_back(unitID); }
    void RemoveUnit(const int unitID) const { UnitIDs->erase(std::remove(UnitIDs->begin(), UnitIDs->end(), unitID), UnitIDs->end()); }
    void AddStructure(const int structureID) const { StructureIDs->push_back(structureID); }
    void RemoveStructure(const int structureID) const { StructureIDs->erase(std::remove(StructureIDs->begin(), StructureIDs->end(), structureID), StructureIDs->end()); }
    void InformCommandCenterDestroyed(const HunterKillerMap& rMap, int commandCenterID);
    const std::string& GetName() const { return Name; }
    int GetID() const { return ID; }
    int GetMapSection() const { return MapSection; }
    int GetCommandCenterID() const { return CommandCenterID; }
    [[nodiscard]] std::vector<int>& GetStructureIDs() const { return *StructureIDs; }
    [[nodiscard]] std::vector<int>& GetUnitIDs() const { return *UnitIDs; }
    int GetScore() const { return Score; }
    [[nodiscard]] OrderStatistics& GetOrderStatistics() const { return *Stats; }
    int GetResource() const { return Resource; }
private:
    const std::string& Name;
    int ID = -1;
    int MapSection = -1;
    int CommandCenterID = -1;
    std::vector<int>* StructureIDs = new std::vector<int>();
    std::vector<int>* UnitIDs = new std::vector<int>();
    int Score = 0;
    OrderStatistics* Stats = new OrderStatistics();
    void SetScore(const int score) { Score = score; }
protected:
    int Resource = 0;
    void SetResource(const int value) { Resource = value; }
    FRIEND_TEST(StructureOrderTest, TestFailSpawnLowResource);
};

