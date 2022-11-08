// ReSharper disable CommentTypo
#include "HunterKillerMoveGenerator.h"

#include "EnumExtensions.h"
#include "Infected.h"
#include "Medic.h"
#include "Soldier.h"
#include "Wall.h"

std::vector<StructureOrder*>* HunterKillerMoveGenerator::GetAllLegalOrders(const HunterKillerState& rState, const Structure& rStructure)
{
    auto* pOrders = new std::vector<StructureOrder*>();

    if (!rState.CanStructureSpawnAUnit(rStructure))
        return pOrders;

    for (const UnitType type : EnumExtensions::GetUnitTypes())
    {
        if (rState.CanStructureSpawn(rStructure, type))
            pOrders->push_back(StructureOrder::Spawn(rStructure, type));
    }

    return pOrders;
}

StructureOrder* HunterKillerMoveGenerator::GetRandomOrder(const HunterKillerState& rState, const Structure& rStructure)
{
    auto unitTypes = EnumExtensions::GetUnitTypes();
    std::ranges::shuffle(unitTypes, HunterKillerConstants::RNG);

    for (const UnitType type : unitTypes)
    {
        if (rState.CanStructureSpawn(rStructure, type))
            return StructureOrder::Spawn(rStructure, type);
    }

    return nullptr;
}

UnitOrder* HunterKillerMoveGenerator::GetRandomOrder(const HunterKillerState& rState, Unit& rUnit)
{
    auto types = EnumExtensions::GetUnitOrderTypes();
    std::ranges::shuffle(types, HunterKillerConstants::RNG);
    const UnitOrderType type = types.front();

    if (!EnumExtensions::NeedsLocation(type))
        return UnitOrder::RotateUnit(rUnit, ROTATE_CLOCKWISE);
                
    if (type == MOVE)
        return GetRandomMoveOrder(rState, rUnit);

    return GetRandomAttackOrder(rState, rUnit, false, type == ATTACK_SPECIAL);
}

std::vector<UnitOrder*>* HunterKillerMoveGenerator::GetAllLegalOrders(const HunterKillerState& rState, Unit& rUnit, const bool includeRotation, const bool includeMovement, const bool includeAttack)
{
    const auto pOrders = new std::vector<UnitOrder*>();

    if (includeMovement) {
        auto pMoveOrders = GetAllLegalMoveOrders(rState, rUnit);
        pOrders->insert(pOrders->end(), std::make_move_iterator(pMoveOrders->begin()), std::make_move_iterator(pMoveOrders->end()));
        delete pMoveOrders;
        pMoveOrders = nullptr;
    }

    if (includeAttack)
    {
        auto pAttackOrders = GetAllLegalAttackOrders(rState, rUnit);
        pOrders->insert(pOrders->end(), std::make_move_iterator(pAttackOrders->begin()), std::make_move_iterator(pAttackOrders->end()));
        delete pAttackOrders;
        pAttackOrders = nullptr;
    }

    if (includeRotation)
    {
        auto pRotationOrders = GetAllLegalRotationOrders(rUnit);
        pOrders->insert(pOrders->end(), std::make_move_iterator(pRotationOrders->begin()), std::make_move_iterator(pRotationOrders->end()));
        delete pRotationOrders;
        pRotationOrders = nullptr;
    }

    return pOrders;
}

std::vector<UnitOrder*>* HunterKillerMoveGenerator::GetAllLegalMoveOrders(const HunterKillerState& rState, const Unit& rUnit)
{
    const auto pOrders = new std::vector<UnitOrder*>();
    const HunterKillerMap& rMap = rState.GetMap();
    const MapLocation& rUnitLocation = rUnit.GetLocation();

    for (const auto direction : EnumExtensions::GetDirections())
    {
        if (const MapLocation* pNewLocation = rMap.GetAdjacentLocationInDirection(rUnitLocation, direction); pNewLocation && rMap.IsMovePossible(rUnitLocation, direction))
            pOrders->push_back(UnitOrder::MoveUnit(rUnit, *pNewLocation));
    }

    return pOrders;
}

UnitOrder* HunterKillerMoveGenerator::GetRandomMoveOrder(const HunterKillerState& rState, const Unit& rUnit)
{
    const HunterKillerMap& rMap = rState.GetMap();
    const MapLocation& rUnitLocation = rUnit.GetLocation();

    auto directions = EnumExtensions::GetDirections();
    std::ranges::shuffle(directions, HunterKillerConstants::RNG);

    for (const auto direction : directions)
    {
        if (const MapLocation* pNewLocation = rMap.GetAdjacentLocationInDirection(rUnitLocation, direction); pNewLocation && rMap.IsMovePossible(rUnitLocation, direction))
           return UnitOrder::MoveUnit(rUnit, *pNewLocation);
    }

    return nullptr;
}

std::vector<UnitOrder*>* HunterKillerMoveGenerator::GetAllLegalRotationOrders(const Unit& rUnit)
{
    const auto pOrders = new std::vector<UnitOrder*>();
    pOrders->push_back(UnitOrder::RotateUnit(rUnit, true));
    pOrders->push_back(UnitOrder::RotateUnit(rUnit, false));
    return pOrders;
}

std::vector<UnitOrder*>* HunterKillerMoveGenerator::GetAllLegalAttackOrders(const HunterKillerState& rState, Unit& rUnit, const bool usePlayersFoV)
{
    const auto pOrders = new std::vector<UnitOrder*>();
    HunterKillerMap& rMap = rState.GetMap();
    const MapLocation& rUnitLocation = rUnit.GetLocation();
    std::unordered_set<MapLocation, MapLocationHash>* pFieldOfView;

    if (usePlayersFoV)
        pFieldOfView = rState.GetPlayer(rUnit.GetControllingPlayerID())->GetCombinedFieldOfView(rMap);
    else
        pFieldOfView = rUnit.GetFieldOfView();

    if (const Infected* pInfected = dynamic_cast<Infected*>(&rUnit); pInfected)
    {
        // Infected can only do melee attacks
        for (const Direction direction : EnumExtensions::GetDirections())
        {
            if (const MapLocation* pTargetLocation = rMap.GetAdjacentLocationInDirection(rUnitLocation, direction); pTargetLocation)
                pOrders->push_back(UnitOrder::UnitAttack(rUnit, *pTargetLocation, false));
        }
        // Also add the Infected's own location as a possibility
        pOrders->push_back(UnitOrder::UnitAttack(rUnit, rUnitLocation, false));
        return pOrders;
    }

    const int attackRange = rUnit.GetAttackRange();
    for (MapLocation location : *pFieldOfView)
    {
        if (MapLocation::GetManhattanDistance(rUnitLocation, location) <= attackRange)
        {
            if (rUnit.GetSpecialAttackCooldown() <= 0) {
                // A Soldier's special attack can't target Walls
                const Soldier* pSoldier = dynamic_cast<Soldier*>(&rUnit);
                if (const Wall* pWall = dynamic_cast<Wall*>(rMap.GetFeatureAtLocation(location)); pSoldier && pWall)
                    continue;

                pOrders->push_back(UnitOrder::UnitAttack(rUnit, location, true));
            }
            pOrders->push_back(UnitOrder::UnitAttack(rUnit, location, false));
        }
    }
    return pOrders;
}

UnitOrder* HunterKillerMoveGenerator::GetRandomAttackOrder(const HunterKillerState& rState, Unit& rUnit, const bool usePlayersFoV, const bool useSpecial)
{
    if (useSpecial && rUnit.GetSpecialAttackCooldown() > 0)
        return nullptr;

    HunterKillerMap& rMap = rState.GetMap();
    const MapLocation& rUnitLocation = rUnit.GetLocation();
    std::unordered_set<MapLocation, MapLocationHash>* pFieldOfView;

    if (usePlayersFoV)
        pFieldOfView = rState.GetPlayer(rUnit.GetControllingPlayerID())->GetCombinedFieldOfView(rMap);
    else
        pFieldOfView = rUnit.GetFieldOfView();

    if (const Infected* pInfected = dynamic_cast<Infected*>(&rUnit); pInfected)
    {
        auto directions = EnumExtensions::GetDirections();
        std::ranges::shuffle(directions, HunterKillerConstants::RNG);
        for (const Direction direction : directions)
        {
            const MapLocation* pTargetLocation = rMap.GetAdjacentLocationInDirection(rUnitLocation, direction);
            if (!pTargetLocation)
                continue;

            if (Unit* pTarget = rMap.GetUnitAtLocation(*pTargetLocation))
            {
                // No sense in killing allied infected
                if (const Infected* pTargetInfected = dynamic_cast<Infected*>(pTarget); pTargetInfected && pTargetInfected->GetControllingPlayerID() == rUnit.GetControllingPlayerID())
                    continue;

                if (usePlayersFoV)
                    delete pFieldOfView;
                return UnitOrder::UnitAttack(rUnit, *pTargetLocation, false);
            }

            if (const Structure* pStructure = dynamic_cast<Structure*>(rMap.GetFeatureAtLocation(*pTargetLocation)); pStructure && pStructure->GetControllingPlayerID() != rUnit.GetControllingPlayerID()) {
                if (usePlayersFoV)
                    delete pFieldOfView;
                return UnitOrder::UnitAttack(rUnit, *pTargetLocation, false);
            }
        }
    }

    const int attackRange = rUnit.GetAttackRange();
    auto* pLocations = new std::vector<MapLocation>();
    for (MapLocation location : *pFieldOfView)
    {
        pLocations->push_back(location);
    }
    std::ranges::shuffle(*pLocations, HunterKillerConstants::RNG);
    for (MapLocation location : *pLocations)
    {
        if (MapLocation::GetManhattanDistance(rUnitLocation, location) > attackRange)
            continue;

        // Check if at this location we'd be targeting either a unit or a mapfeature
        IControlled* pTarget = rMap.GetUnitAtLocation(location);
        MapFeature* pFeature = nullptr;
        if (!pTarget)
        {
            pFeature = rMap.GetFeatureAtLocation(location);
            if (auto* pControlledFeature = dynamic_cast<IControlled*>(pFeature); pControlledFeature)
                pTarget = pControlledFeature;
        }

        // If we couldn't find a target, or if we found an allied mapfeature as target, continue
        if (!pTarget || (pFeature && pTarget->GetControllingPlayerID() == rUnit.GetControllingPlayerID()))
            continue;

        if (useSpecial)
        {
            // A Soldier's special attack can't target Walls
            const Soldier* pSoldier = dynamic_cast<Soldier*>(&rUnit);
            if (const Wall* pWall = dynamic_cast<Wall*>(rMap.GetFeatureAtLocation(location)); pSoldier && pWall)
                continue;

            // Do not target friendlies if we aren't a medic.
            const Medic* pMedic = dynamic_cast<Medic*>(&rUnit);
            if (!pMedic && pTarget->GetControllingPlayerID() == rUnit.GetControllingPlayerID())
                continue;
            // Do not heal enemies
            if (pMedic && pTarget->GetControllingPlayerID() != rUnit.GetControllingPlayerID())
                continue;
            // Do not heal non-damaged units
            if (const Unit* pTargetUnit = dynamic_cast<Unit*>(pTarget); pMedic && pTargetUnit && !pTargetUnit->IsDamaged())
                continue;

            return UnitOrder::UnitAttack(rUnit, location, true);
        }

        // Don't target friendlies
        if (pTarget->GetControllingPlayerID() == rUnit.GetControllingPlayerID())
            continue;

        if (usePlayersFoV)
            delete pFieldOfView;
        delete pLocations;
        return UnitOrder::UnitAttack(rUnit, location, false);
    }

    if (usePlayersFoV)
        delete pFieldOfView;
    delete pLocations;
    return nullptr;
}
