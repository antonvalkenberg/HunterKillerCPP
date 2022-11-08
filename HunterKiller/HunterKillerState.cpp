// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "HunterKillerState.h"

#include "EnumExtensions.h"
#include "Space.h"

HunterKillerState::HunterKillerState(const HunterKillerState& rHKState)
{
    CurrentRound = rHKState.GetCurrentRound();
    TurnCounter = rHKState.TurnCounter;
    ActivePlayerID = rHKState.GetActivePlayerID();
    Map = rHKState.GetMap().Copy();
    Players = new std::vector<HunterKillerPlayer*>();
    for (const HunterKillerPlayer* pPlayer : *rHKState.GetPlayers())
    {
        Players->push_back(pPlayer->Copy());
    }
}

HunterKillerState::~HunterKillerState()
{
    delete Map;
    Map = nullptr;
    for (const HunterKillerPlayer* pPlayer : *Players)
    {
        delete pPlayer;
        pPlayer = nullptr;
    }
    delete Players;
    Players = nullptr;
}

HunterKillerState* HunterKillerState::Copy() const
{
    return new HunterKillerState(*this);
}

int HunterKillerState::GetHashCode() const
{
    int hashcode = 43;
    hashcode ^= CurrentRound ^ TurnCounter ^ Map->GetHashCode();
    if (Players)
    {
        for (const HunterKillerPlayer* pPlayer : *Players)
        {
            hashcode ^= pPlayer->GetHashCode();
        }
    }
    return hashcode;
}

HunterKillerPlayer* HunterKillerState::GetPlayer(const int playerID) const
{
    for (HunterKillerPlayer* pPlayer : *Players) {
        if (pPlayer && pPlayer->GetID() == playerID)
            return pPlayer;
    }
    return nullptr;
}

std::unordered_map<int, int>* HunterKillerState::GetScores() const
{
    auto* pScores = new std::unordered_map<int, int>();
    for (const HunterKillerPlayer* pPlayer : *Players)
    {
        pScores->insert(std::pair(pPlayer->GetID(), pPlayer->GetScore()));
    }
    return pScores;
}

bool HunterKillerState::IsDone() const
{
    // A game is completed once only 1 command center remains, or if we have reached the maximum allowed number of rounds and the last player has made their move
    return Map->GetCurrentCommandCenterCount() == 1 || (CurrentRound >= HunterKillerConstants::RULES_MAX_GAME_ROUNDS && ActivePlayerID == Players->back()->GetID());
}

void HunterKillerState::EndPlayerTurn()
{
    ++TurnCounter;
    ActivePlayerID = Players->at(TurnCounter)->GetID();
    
    if (TurnCounter % GetNumberOfPlayers() == 0)
    {
        Map->Timer();
        ++CurrentRound;

        if (CurrentRound % HunterKillerConstants::RULES_STRUCTURE_GENERATION_FREQUENCY == 0)
        {
            for (const std::unordered_map<int, GameObject*>* objects = Map->GetObjects(); std::pair<int, GameObject*> pGameObjectPair : *objects)
            {
                const Structure* pStructure = dynamic_cast<Structure*>(pGameObjectPair.second);
                if (pStructure && pStructure->GetGeneratesResource())
                    AwardResourcesToController(*pStructure);
                if (pStructure && pStructure->GetGeneratesScore())
                    AwardScoreToController(*pStructure);
            }
        }
    }
    MapCleanup();
}

void HunterKillerState::MapCleanup() const
{
    const std::unordered_map<int, GameObject*>* pObjects = Map->GetObjects();
    if (!pObjects) return;

    // Make a collection of object-IDs to check, so that we don't iterate over a collection that can/will be modified
    auto* pObjectIDs = new std::vector<int>();
    for (int objectID : std::views::keys(*pObjects))
    {
        pObjectIDs->push_back(objectID);
    }

    for (const int objectID : *pObjectIDs) {
        auto* pObject = Map->GetObject(objectID);
        if (!pObject || !ShouldGameObjectBeRemoved(pObject))
            continue;

        const int mapPosition = Map->ToPosition(pObject->GetLocation());

        Map->UnregisterGameObject(pObject);

        if (const auto pStructure = dynamic_cast<Structure*>(pObject)) {
            if (pStructure->IsUnderControl()) {
                if (HunterKillerPlayer* pPlayer = GetPlayer(pStructure->GetControllingPlayerID())) {
                    if (pStructure->GetIsCommandCenter())
                        pPlayer->InformCommandCenterDestroyed(*Map, pStructure->GetID());
                    else
                        pPlayer->RemoveStructure(pStructure->GetID());
                }
            }

            const auto pSpace = new Space(Map->ToLocation(mapPosition));
            Map->RegisterGameObject(pSpace);
            Map->Place(mapPosition, pSpace);
        }
        else if (const auto pUnit = dynamic_cast<Unit*>(pObject)) {
            if (const HunterKillerPlayer* pPlayer = GetPlayer(pUnit->GetControllingPlayerID()))
                pPlayer->RemoveUnit(pUnit->GetID());
        }

        // Release the memory of the removed object
        delete pObject; pObject = nullptr;
        //TODO: not happy with this being here, HunterKillerMap has ownership of GameObjects' memory. Should/Could go into UnregisterGameObject, but after that call we still need the object's data to tell the player that it has been removed.
    }
    delete pObjectIDs; pObjectIDs = nullptr;

    Map->UpdateFieldOfView();
}

bool HunterKillerState::ShouldGameObjectBeRemoved(GameObject* pGameObject) const
{
    if (const Unit* pUnit = dynamic_cast<Unit*>(pGameObject); pUnit)
        return pUnit->GetCurrentHP() <= 0;

    if (const MapFeature* pMapFeature = dynamic_cast<MapFeature*>(pGameObject); pMapFeature)
    {
        if (auto* pStructure = dynamic_cast<Structure*>(pGameObject); pStructure) {
            if (pStructure->GetIsDestructible() && pStructure->GetCurrentHP() <= 0)
                return true;

            if (pStructure->GetIsCapturable())
            {
                if (const Unit* pUnit = Map->GetUnitAtLocation(pStructure->GetLocation()); pUnit && pUnit->GetControllingPlayerID() != pStructure->GetControllingPlayerID())
                {
                    if (const HunterKillerPlayer* pCurrentController = GetPlayer(pStructure->GetControllingPlayerID()); pStructure->IsUnderControl() && pCurrentController)
                        pCurrentController->RemoveStructure(pStructure->GetID());

                    const HunterKillerPlayer* pNewController = GetPlayer(pUnit->GetControllingPlayerID());
                    pStructure->SetControllingPlayerID(pNewController->GetID());
                    pNewController->AddStructure(pStructure->GetID());
                }
            }
        }

        return pMapFeature->GetIsDestructible() && pMapFeature->GetCurrentHP() <= 0;
    }

    return false;
}

bool HunterKillerState::IsStructureSpawnAreaFree(const Structure& rStructure) const
{
    const MapLocation* pSpawnLocation = rStructure.GetSpawnLocation();
    return pSpawnLocation && Map->IsTraversable(*pSpawnLocation);
}

bool HunterKillerState::CanStructureSpawnAUnit(const Structure& rStructure) const
{
    return std::ranges::any_of(EnumExtensions::GetUnitTypes(), [&](const UnitType type) { return CanStructureSpawn(rStructure, type); });
}

bool HunterKillerState::CanStructureExecute(const Structure& rStructure, const StructureOrderType structureOrderType) const
{
    switch (structureOrderType) {
    case SPAWN_INFECTED:
        return CanStructureSpawn(rStructure, UNIT_INFECTED);
    case SPAWN_MEDIC:
        return CanStructureSpawn(rStructure, UNIT_MEDIC);
    case SPAWN_SOLDIER:
        return CanStructureSpawn(rStructure, UNIT_SOLDIER);
    default:
        return false;
    }
}

bool HunterKillerState::CanStructureSpawn(const Structure& rStructure, const UnitType unitType) const
{
    if (!rStructure.IsUnderControl())
        return false;

    // Get the player's resource
    const int playerResource = GetPlayer(rStructure.GetControllingPlayerID())->GetResource();
    // Check if the resource amount is at least the cost to spawn the specified unit type
    return rStructure.GetAllowsSpawning() && IsStructureSpawnAreaFree(rStructure) && playerResource >= Unit::GetSpawnCost(unitType);
}

void HunterKillerState::AwardResourcesToController(const Structure& rStructure) const
{
    if (HunterKillerPlayer* pPlayer = GetPlayer(rStructure.GetControllingPlayerID()); pPlayer && rStructure.GetGeneratesResource())
        pPlayer->AwardResource(rStructure.GetResourceGeneration());
}

void HunterKillerState::AwardScoreToController(const Structure& rStructure) const
{
    if (HunterKillerPlayer* pPlayer = GetPlayer(rStructure.GetControllingPlayerID()); pPlayer && rStructure.GetGeneratesScore())
        pPlayer->AwardScore(rStructure.GetScoreGeneration());
}
