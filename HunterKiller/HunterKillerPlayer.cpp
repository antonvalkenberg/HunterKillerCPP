#include "HunterKillerPlayer.h"

HunterKillerPlayer::~HunterKillerPlayer()
{
    delete StructureIDs;
    StructureIDs = nullptr;
    delete UnitIDs;
    UnitIDs = nullptr;
    delete Stats;
    Stats = nullptr;
}

HunterKillerPlayer* HunterKillerPlayer::Copy() const
{
    auto* pNewPlayer = new HunterKillerPlayer(ID, Name, MapSection, Resource);

    pNewPlayer->CommandCenterID = CommandCenterID;
    pNewPlayer->StructureIDs = new std::vector(*StructureIDs);
    pNewPlayer->UnitIDs = new std::vector(*UnitIDs);
    pNewPlayer->SetScore(Score);

    return pNewPlayer;
}

std::unordered_set<MapLocation, MapLocationHash>* HunterKillerPlayer::GetCombinedFieldOfView(HunterKillerMap& rMap) const
{
    auto* pFieldOfViewSet = new std::unordered_set<MapLocation, MapLocationHash>();
    for (const int structureID : *StructureIDs) {
        if (const Structure* pStructure = dynamic_cast<Structure*>(rMap.GetObject(structureID))) {
            std::unordered_set<MapLocation, MapLocationHash>* pStructFoV = rMap.GetFieldOfView(*pStructure);
            pFieldOfViewSet->insert(pStructFoV->begin(), pStructFoV->end());
        }
    }
    for (const int unitID : *UnitIDs) {
        if (const Unit* pUnit = dynamic_cast<Unit*>(rMap.GetObject(unitID))) {
            std::unordered_set<MapLocation, MapLocationHash>* pUnitFoV = rMap.GetFieldOfView(*pUnit);
            pFieldOfViewSet->insert(pUnitFoV->begin(), pUnitFoV->end());
        }
    }
    return pFieldOfViewSet;
}

std::vector<Structure*>* HunterKillerPlayer::GetStructures(const HunterKillerMap& rMap) const
{
    auto* pStructures = new std::vector<Structure*>();
    for (const int structureID : *StructureIDs) {
        if (auto* pStructure = dynamic_cast<Structure*>(rMap.GetObject(structureID)))
            pStructures->push_back(pStructure);
    }
    return pStructures;
}

std::vector<Unit*>* HunterKillerPlayer::GetUnits(const HunterKillerMap& rMap) const
{
    auto* pUnits = new std::vector<Unit*>();
    for (const int unitID : *UnitIDs) {
        if (auto* pUnit = dynamic_cast<Unit*>(rMap.GetObject(unitID)))
            pUnits->push_back(pUnit);
    }
    return pUnits;
}

void HunterKillerPlayer::InformCommandCenterDestroyed(const HunterKillerMap& rMap, const int commandCenterID)
{
    if (CommandCenterID != commandCenterID) return;
    
    RemoveStructure(commandCenterID);

    // When a player leaves the game, their structures are not removed from the map
    for (const std::vector<Structure*>* pStructures = GetStructures(rMap); Structure* pStructure : *pStructures) {
        pStructure->SetControllingPlayerID(HunterKillerConstants::STRUCTURE_NO_CONTROL);
    }
    StructureIDs->clear();

    // But their units however, are removed
    for (const std::vector<Unit*>* pUnits = GetUnits(rMap); Unit* pUnit : *pUnits) {
        rMap.UnregisterGameObject(pUnit);
        delete pUnit;
        pUnit = nullptr;
    }
    UnitIDs->clear();

    // Half the player's score because their command center got destroyed
    Score = Score / 2;
    //Score >>= 1;
}
