// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
// ReSharper disable StringLiteralTypo
// ReSharper disable CommentTypo
#include "HunterKillerRules.h"

#include "Infected.h"
#include "Medic.h"
#include "Soldier.h"
#include "StructureOrder.h"
#include "Wall.h"
#include "EnumExtensions.h"

Result* HunterKillerRules::Handle(HunterKillerState& rState, const HunterKillerAction& rAction)
{
    if (rAction.GetActingPlayerID() != rState.GetActivePlayerID()) {
        const auto* pInformationString = new std::string(std::format("Player performing the Action (ID: {0:d}) is not the active player (ID: {1:d}).", rAction.GetActingPlayerID(), rState.GetActivePlayerID()));
        return new Result(false, pInformationString);
    }
    if (rAction.GetCurrentRound() != rState.GetCurrentRound()) {
        const auto* pInformationString = new std::string(std::format("Round number of the Action ({0:d}) did not match the State's ({1:d})", rAction.GetCurrentRound(), rState.GetCurrentRound()));
        return new Result(false, pInformationString);
    }

    Result* pActionResult = PerformAction(rState, rAction);
    // We need to check if the state is a completed state before ending the turn, because ending the turn rolls over some variables like turncounter and activeplayer
    bool stateIsDone = rState.IsDone();
    rState.EndPlayerTurn();

    if (stateIsDone)
    {
        // Sort players by descending score
        auto* pPlayers = rState.GetPlayers();
        std::ranges::sort(*pPlayers, [](const HunterKillerPlayer* a, const HunterKillerPlayer* b) {return a->GetScore() > b->GetScore(); });
        // Create ranking
        auto* pPlayerRanking = new std::vector<std::pair<int, int>>();
        for (const auto pPlayer : *pPlayers)
        {
            pPlayerRanking->push_back(std::pair(pPlayer->GetID(), pPlayer->GetScore()));
        }
        // Return result including ranking
        pActionResult->PlayerRanking = pPlayerRanking;
        pActionResult->FinishedGame = true;
    }

    return pActionResult;
}

Result* HunterKillerRules::PerformAction(const HunterKillerState& rState, const HunterKillerAction& rAction)
{
    int failCount = 0;
    std::string* pActionFailures = IGNORE_FAILURES ? nullptr : new std::string;

    const std::vector<HunterKillerOrder*>* pOrders = rAction.GetOrders();
    rState.GetActivePlayer().Stats->Issued += static_cast<int>(pOrders->size());

    for (HunterKillerOrder* pOrder : *pOrders)
    {
        std::string* pOrderFailures = IGNORE_FAILURES ? nullptr : new std::string;

        if (!IsOrderPossible(rState, *pOrder, pOrderFailures))
        {
            ++failCount;
            pOrder->SetAccepted(false);
            if (pActionFailures)
                *pActionFailures += *pOrderFailures;
            delete pOrderFailures; pOrderFailures = nullptr;
            continue;
        }

        ExecuteOrder(rState, *pOrder, pOrderFailures);

        if (pOrderFailures && !pOrderFailures->empty())
        {
            ++failCount;
            pOrder->SetAccepted(false);
            if (pActionFailures)
                *pActionFailures += *pOrderFailures;
            delete pOrderFailures; pOrderFailures = nullptr;
            continue;
        }

        pOrder->SetAccepted(true);
        rState.GetActivePlayer().Stats->Success++;
    }

    rState.GetActivePlayer().Stats->Failed += failCount;

    // ReSharper disable CppRedundantBooleanExpressionArgument
    if (LOG_TO_CONSOLE && pActionFailures && failCount > 0)
        // ReSharper restore CppRedundantBooleanExpressionArgument
        std::cout << std::format("P({0:d})R({1:d}): {2:d} orders failed, reasons: \n{3:s}\n", rAction.GetActingPlayerID(), rState.GetCurrentRound(), failCount, *pActionFailures);

    const auto* pInformation = new std::string(pActionFailures && failCount > 0 ? std::format("{0:d} orders failed, reasons: \n{1:s}\n", failCount, *pActionFailures) : "");
    return new Result(true, pInformation);
}

void HunterKillerRules::ExecuteOrder(const HunterKillerState& rState, HunterKillerOrder& rOrder, std::string* pFailureReasons)
{
    if (!IsOrderPossible(rState, rOrder, pFailureReasons))
        return;

    HunterKillerMap& rMap = rState.GetMap();
    HunterKillerPlayer& rActivePlayer = rState.GetActivePlayer();
    OrderStatistics& rStats = rActivePlayer.GetOrderStatistics();
    GameObject* pOrderObject = rMap.GetObject(rOrder.GetObjectID());

    if (const StructureOrder* pStructureOrder = dynamic_cast<StructureOrder*>(&rOrder); pStructureOrder)
    {
        const StructureOrderType type = pStructureOrder->GetOrderType();
        const Structure* pStructure = dynamic_cast<Structure*>(pOrderObject);
        MapLocation* pSpawnLocation = pStructure->GetSpawnLocation();
        const Direction spawnDirection = MapLocation::GetDirectionTo(pStructure->GetLocation(), *pSpawnLocation).value();
        int spawnCosts;
        Unit* pUnit;

        switch (type)
        {
        case SPAWN_INFECTED:
            pUnit = new Infected(rActivePlayer.GetID(), *pSpawnLocation, spawnDirection);
            spawnCosts = HunterKillerConstants::INFECTED_SPAWN_COST;
            ++rStats.SpawnInfected;
            break;
        case SPAWN_MEDIC:
            pUnit = new Medic(rActivePlayer.GetID(), *pSpawnLocation, spawnDirection);
            spawnCosts = HunterKillerConstants::MEDIC_SPAWN_COST;
            ++rStats.SpawnMedic;
            break;
        case SPAWN_SOLDIER:
            pUnit = new Soldier(rActivePlayer.GetID(), *pSpawnLocation, spawnDirection);
            spawnCosts = HunterKillerConstants::SOLDIER_SPAWN_COST;
            ++rStats.SpawnSoldier;
            break;
        default:
            if (pFailureReasons)
                *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: Unrecognized order type {1:d}.\n", pStructureOrder->GetObjectID(), static_cast<int>(type));
            return;
        }

        if (rMap.Place(*pSpawnLocation, pUnit))
        {
            rActivePlayer.SetResource(rActivePlayer.GetResource() - spawnCosts);
            rMap.RegisterGameObject(pUnit);
            rActivePlayer.AddUnit(pUnit->GetID());
            pUnit->UpdateFieldOfView(rMap.GetFieldOfView(*pUnit));
        }
        else
        {   // Cleanup the unit we couldn't place
            delete pUnit;
            pUnit = nullptr;
        }
    }
    else if (const UnitOrder* pUnitOrder = dynamic_cast<UnitOrder*>(&rOrder); pUnitOrder) {
        const UnitOrderType type = pUnitOrder->GetOrderType();
        auto* pUnit = dynamic_cast<Unit*>(pOrderObject);
        std::optional<MapLocation> targetLocation = pUnitOrder->GetTargetLocation();

        switch (type)
        {
        case ROTATE_CLOCKWISE:
            pUnit->SetOrientation(EnumExtensions::TryRotate(pUnit->GetOrientation(), CLOCKWISE));
            pUnit->InvalidateFieldOfView();
            ++rStats.RotateClockwise;
            break;
        case ROTATE_COUNTER_CLOCKWISE:
            pUnit->SetOrientation(EnumExtensions::TryRotate(pUnit->GetOrientation(), COUNTER_CLOCKWISE));
            pUnit->InvalidateFieldOfView();
            ++rStats.RotateCounter;
            break;
        case MOVE:
            rMap.Move(targetLocation.value(), *pUnit, pFailureReasons);
            pUnit->InvalidateFieldOfView();
            ++rStats.Move;
            break;
        case ATTACK:
            {
            const bool attackSuccess = rMap.AttackLocation(targetLocation.value(), pUnit->GetAttackDamage());
            ++rStats.Attack;

            // Check if we need to trigger an Infected's special attack.
            // Several conditions need to hold: (in order of most likely to break out of the statement)
            // - An Infected was the source of the attack
            // - There is a unit on the targeted location
            // - The target is not an Infected
            // - The target is now dead
            // - The Infected's special attack is not on cooldown
            // - The attack succeeded
            Unit* pTargetUnit = rMap.GetUnitAtLocation(targetLocation.value());
            const Infected* pSourceInfected = dynamic_cast<Infected*>(pUnit);
            const Infected* pTargetInfected = dynamic_cast<Infected*>(pTargetUnit);
            bool shouldReleaseTargetUnitMemory = false;
            if (pSourceInfected && pTargetUnit && !pTargetInfected && pTargetUnit->GetCurrentHP() <= 0 && pUnit->GetSpecialAttackCooldown() == 0 && attackSuccess) {

                rState.GetPlayer(pTargetUnit->GetControllingPlayerID())->RemoveUnit(pTargetUnit->GetID());
                rMap.UnregisterGameObject(pTargetUnit);
                AwardPointsForUnitDeath(rActivePlayer, *pTargetUnit);
                shouldReleaseTargetUnitMemory = true;

                // Spawn a new Infected, on the same team as the Infected that performed this attack
                auto* pSpawn = new Infected(rActivePlayer.GetID(), targetLocation.value(), pUnit->GetOrientation());
                rMap.RegisterGameObject(pSpawn);
                rMap.Place(targetLocation.value(), pSpawn);

                rActivePlayer.AddUnit(pSpawn->GetID());
                pSpawn->UpdateFieldOfView(rMap.GetFieldOfView(*pSpawn));

                pUnit->StartCooldown();
            }
            // Otherwise, check if there was a Unit on the targeted location, and if it is currently dead
            else if (pTargetUnit && pTargetUnit->GetCurrentHP() <= 0) {
                // Award points to the player
                AwardPointsForUnitDeath(rActivePlayer, *pTargetUnit);
                // NOTE: we don't have to release the unit's memory here, because we didn't have to do an Unregister->Register loop like above
                // It'll get cleaned up in the Map's cleanup
            }

            if (pTargetUnit) {
                ++rStats.AttackUnit;
                if (pTargetUnit->IsControlledBy(rActivePlayer.GetID()))
                    ++rStats.AttackAlly;
            }
            if (const Structure* pStructure = dynamic_cast<Structure*>(rMap.GetFeatureAtLocation(targetLocation.value())); pStructure)
                ++rStats.AttackStructure;

            if (shouldReleaseTargetUnitMemory)
            {
                delete pTargetUnit;
                pTargetUnit = nullptr;
            }
            }
            break;
        case ATTACK_SPECIAL:
            switch (pUnit->GetType())
            {
            case UNIT_INFECTED:
                break;
            case UNIT_MEDIC:
                rMap.GetUnitAtLocation(targetLocation.value())->IncreaseHP(HunterKillerConstants::MEDIC_SPECIAL_HEAL);
                pUnit->StartCooldown();
                ++rStats.Heal;
                break;
            case UNIT_SOLDIER:
                {
                // The special attack of a soldier is a grenade that does damage in an area
                auto* areaOfEffect = new std::unordered_set<MapLocation, MapLocationHash>();
                rMap.GetAreaAround(targetLocation.value(), true, *areaOfEffect);
                for (MapLocation location : *areaOfEffect) {
                    // Call an attack on each location inside the area of effect
                    if (rMap.AttackLocation(location, HunterKillerConstants::SOLDIER_SPECIAL_DAMAGE)) {
                        // Check if there was a Unit on the targeted location, and if it is currently dead
                        if (Unit* pAoEUnit = rMap.GetUnitAtLocation(location); pAoEUnit && pAoEUnit->GetCurrentHP() <= 0) {
                            // Award points to the player
                            AwardPointsForUnitDeath(rActivePlayer, *pAoEUnit);
                        }
                    }
                }
                pUnit->StartCooldown();
                ++rStats.Grenade;
                delete areaOfEffect;
                areaOfEffect = nullptr;
                }
                break;
            default:
                // Getting here means we have come across a type that is not yet implemented
                if (pFailureReasons)
                    *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Unrecognized unit type.\n", pUnitOrder->GetObjectID());
            }
            break;
        default:
            // Getting here means we have come across a type that is not yet implemented
            if (pFailureReasons)
                *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Unrecognized order type {1:d}.\n", pUnitOrder->GetObjectID(), static_cast<int>(type));
        }
    }
    else {
        // Getting here means we have come across a type that is not yet implemented
        if (pFailureReasons)
            *pFailureReasons += std::format("Order fail for ID {0:d}: Unsupported order type.\n", rOrder.GetObjectID());
    }
}

bool HunterKillerRules::IsOrderPossible(const HunterKillerState& rState, HunterKillerOrder& rOrder, std::string* pFailureReasons)
{
    const HunterKillerMap& rMap = rState.GetMap();

    // Check if the object that the order is for still exists
    GameObject* pOrderObject = rMap.GetObject(rOrder.GetObjectID());
    if (!pOrderObject)
    {
        if (pFailureReasons)
            *pFailureReasons += std::format("Order fail: Could not find object with ID {0:d}.\n", rOrder.GetObjectID());
        return false;
    }

    // Check if the object is being controlled by the currently active player
    if (const IControlled* pControlledObject = dynamic_cast<IControlled*>(pOrderObject); pControlledObject && !pControlledObject->IsControlledBy(rState.GetActivePlayerID()))
    {
        if (pFailureReasons)
            *pFailureReasons += std::format("Order fail: Active player ({0:d}) does not control object with ID {1:d}.\n", rState.GetActivePlayerID(), rOrder.GetObjectID());
        return false;
    }

    // Check which type of order we are dealing with
    if (const StructureOrder* pStructureOrder = dynamic_cast<StructureOrder*>(&rOrder); pStructureOrder)
    {
        // Make sure the order-object is a structure
        const Structure* pStructureObject = dynamic_cast<Structure*>(pOrderObject);
        if (!pStructureObject) {
            if (pFailureReasons)
                *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: object is not a Structure.\n", pOrderObject->GetID());
            return false;
        }

        if (!pStructureObject->GetAllowsSpawning())
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: Structure does not allow spawning.\n", pOrderObject->GetID());
            return false;
        }

        const MapLocation* pSpawnLocation = pStructureObject->GetSpawnLocation();
        if (!pSpawnLocation || !rMap.IsTraversable(*pSpawnLocation, pFailureReasons))
            return false;

        // Check if a spawn direction can be determined
        if (!MapLocation::GetDirectionTo(pStructureObject->GetLocation(), *pSpawnLocation).has_value())
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: Spawn location ({1:s}) is not on a cardinal direction relative to the structure.\n", pStructureObject->GetID(), pSpawnLocation->ToString());
            return false;
        }

        switch (const StructureOrderType orderType = pStructureOrder->GetOrderType())
        {
        case SPAWN_INFECTED:
        case SPAWN_MEDIC:
        case SPAWN_SOLDIER:
            // Check if the player has enough resources to spawn this type
            if (!rState.CanStructureExecute(*pStructureObject, orderType))
            {
                if (pFailureReasons)
                    *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: Insufficient resources available for order of type {1:d}.\n", pStructureObject->GetID(), static_cast<int>(orderType));
                return false;
            }
            return true;
        default:
            if (pFailureReasons)
                *pFailureReasons += std::format("StructureOrder fail for ID {0:d}: Unsupported order type {1:d}.\n", pStructureObject->GetID(), static_cast<int>(orderType));
            return false;
        }
    }

    if (const UnitOrder* pUnitOrder = dynamic_cast<UnitOrder*>(&rOrder); pUnitOrder)
    {
        // Make sure that the order-object is a unit
        const Unit* pUnit = dynamic_cast<Unit*>(pOrderObject);
        if (!pUnit)
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Source object is not a Unit.\n", pUnitOrder->GetObjectID());
            return false;
        }

        const UnitOrderType type = pUnitOrder->GetOrderType();
        // Rotations don't need any other checks
        if (type == ROTATE_CLOCKWISE || type == ROTATE_COUNTER_CLOCKWISE)
            return true;

        // Make sure a target location has been set
        const std::optional<MapLocation> targetLocation = pUnitOrder->GetTargetLocation();
        if (!targetLocation.has_value())
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: No target location set.\n", pUnitOrder->GetObjectID());
            return false;
        }

        if (type == MOVE)
            return rMap.IsMovePossible(pOrderObject->GetLocation(), *pUnitOrder, pFailureReasons);

        // Make sure the target location is in the Unit's field of view
        if (!pUnit->IsInFieldOfView(targetLocation.value()))
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("UnitOrder ({0:d} -> Attack {1:s}) fail: Target location is not in unit's Field-of-View.\n", pUnitOrder->GetObjectID(), targetLocation.value().ToString());
            return false;
        }

        // Check if the target location is within the Unit's attack range
        if (pUnit->GetAttackRange() < MapLocation::GetManhattanDistance(pUnit->GetLocation(), targetLocation.value()))
        {
            if (pFailureReasons)
                *pFailureReasons += std::format("UnitOrder ({0:d} -> Attack {1:s}) fail: Target location is outside of attack range.\n", pUnitOrder->GetObjectID(), targetLocation.value().ToString());
            return false;
        }

        // Attacks don't need any other checks
        if (type == ATTACK)
            return true;

        if (type == ATTACK_SPECIAL) {
            //Make sure the Unit's cooldown is available
            if (pUnit->GetSpecialAttackCooldown() > 0)
            {
                if (pFailureReasons)
                    *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Special attack is still on cooldown ({1:d} round(s) remaining).\n", pUnitOrder->GetObjectID(), pUnit->GetSpecialAttackCooldown());
                return false;
            }

            switch (pUnit->GetType())
            {
            case UNIT_SOLDIER:
                // The special of a Soldier can't have a Wall as it's target
                if (const Wall* pWall = dynamic_cast<Wall*>(rMap.GetFeatureAtLocation(targetLocation.value())); pWall)
                {
                    if (pFailureReasons)
                        *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: A Soldier's special attack cannot target a Wall.\n", pUnitOrder->GetObjectID());
                    return false;
                }
                return true;
            case UNIT_MEDIC:
                // Fail if the target location of a Medic's special doesn't contain a Unit
                if (!rMap.GetUnitAtLocation(targetLocation.value()))
                {
                    if (pFailureReasons)
                        *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Target location does not contain a Unit to heal.\n", pUnitOrder->GetObjectID());
                    return false;
                }
                return true;
            case UNIT_INFECTED:
                // The special attack of an infected can't be ordered, since it triggers on kill
                if (pFailureReasons)
                    *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: An Infected's special attack cannot be ordered.\n", pUnitOrder->GetObjectID());
                return false;
            default:
                if (pFailureReasons)
                    *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Unrecognized UnitType.\n", pUnitOrder->GetObjectID());
                return false;
            }
        }

        if (pFailureReasons)
            *pFailureReasons += std::format("UnitOrder fail for ID {0:d}: Unsupported order type {1:d}.\n", pUnitOrder->GetObjectID(), static_cast<int>(type));
        return false;
    }

    if (pFailureReasons)
        *pFailureReasons += "Order fail: Unrecognized order type.\n";
    return false;
}

bool HunterKillerRules::AddAndExecuteOrderIfPossible(const HunterKillerAction& rAction, const HunterKillerState& rState, HunterKillerOrder* pOrder, std::string* pPossibleCheckFails, std::string* pOrderFails)
{
    if (IsOrderPossible(rState, *pOrder, pPossibleCheckFails))
    {
        ExecuteOrder(rState, *pOrder, pOrderFails);

        return rAction.TryAddOrder(pOrder);
    }

    return false;
}

void HunterKillerRules::AwardPointsForUnitDeath(HunterKillerPlayer& rPlayer, Unit& rKilledUnit)
{
    // Only award points if the unit was not controlled by the player itself
    if (rKilledUnit.GetControllingPlayerID() == rPlayer.GetID())
        return;

    if (const Soldier* pSoldier = dynamic_cast<Soldier*>(&rKilledUnit); pSoldier)
    {
        rPlayer.AwardScore(HunterKillerConstants::SOLDIER_SCORE);
        return;
    }
    if (const Medic* pMedic = dynamic_cast<Medic*>(&rKilledUnit); pMedic)
    {
        rPlayer.AwardScore(HunterKillerConstants::MEDIC_SCORE);
        return;
    }
    if (const Infected* pInfected = dynamic_cast<Infected*>(&rKilledUnit); pInfected)
        rPlayer.AwardScore(HunterKillerConstants::INFECTED_SCORE);
}
