#include "pch.h"
#include "SlightlyRandomBot.h"
#include "../HunterKiller/HunterKillerMoveGenerator.h"

SlightlyRandomBot::~SlightlyRandomBot()
{
    delete BotName; BotName = nullptr;
}

HunterKillerAction* SlightlyRandomBot::CreateSlightlyRandomAction(const HunterKillerState& rState)
{
    std::uniform_real_distribution<double> random_double(0, 1);
    auto* pRandomAction = new HunterKillerAction(rState);
    const auto& rPlayer = rState.GetActivePlayer();
    const auto& rMap = rState.GetMap();
    const auto* pStructures = rPlayer.GetStructures(rMap);
    const auto* pUnits = rPlayer.GetUnits(rMap);

    for (const auto* pStructure : *pStructures)
    {
        if (random_double(HunterKillerConstants::RNG) <= NO_BASE_ORDER_THRESHOLD)
            continue;

        if (StructureOrder* pOrder = GetRandomOrder(rState, *pStructure))
            pRandomAction->TryAddOrder(pOrder);
    }
    for (auto* pUnit : *pUnits)
    {
        if (random_double(HunterKillerConstants::RNG) <= NO_UNIT_ORDER_THRESHOLD)
            continue;

        if (UnitOrder* pOrder = CreateSlightlyRandomOrder(rState, pUnit))
            pRandomAction->TryAddOrder(pOrder);
    }

    delete pStructures; pStructures = nullptr;
    delete pUnits; pUnits = nullptr;
    return pRandomAction;
}

UnitOrder* SlightlyRandomBot::CreateSlightlyRandomOrder(const HunterKillerState& rState, Unit* pUnit)
{
    auto* pLegalRotationOrders = HunterKillerMoveGenerator::GetAllLegalRotationOrders(*pUnit);
    auto* pLegalMoveOrders = HunterKillerMoveGenerator::GetAllLegalMoveOrders(rState, *pUnit);
    auto* pLegalAttackOrders = HunterKillerMoveGenerator::GetAllLegalAttackOrders(rState, *pUnit);

    FilterFriendlyFire(*pLegalAttackOrders, pUnit, rState.GetMap(), false);

    std::uniform_real_distribution<double> random_double(0, 1);
    const double attackType = random_double(HunterKillerConstants::RNG);

    // Do a random rotation with 20% chance
    if (attackType <= 0.2 && !pLegalRotationOrders->empty())
    {
        std::ranges::shuffle(*pLegalRotationOrders, HunterKillerConstants::RNG);
        auto* pOrder = pLegalRotationOrders->at(0);
        for (size_t i = 1; i < pLegalRotationOrders->size(); ++i)
        {
            delete pLegalRotationOrders->at(i);
        }
        delete pLegalRotationOrders; pLegalRotationOrders = nullptr;
        for (const auto* pLegalMoveOrder : *pLegalMoveOrders)
        {
            delete pLegalMoveOrder;
        }
        delete pLegalMoveOrders; pLegalMoveOrders = nullptr;
        for (const auto* pLegalAttackOrder : *pLegalAttackOrders)
        {
            delete pLegalAttackOrder;
        }
        delete pLegalAttackOrders; pLegalAttackOrders = nullptr;
        return pOrder;
    }
    // Do a random move with 50% chance
    if (attackType <= 0.7 && !pLegalMoveOrders->empty())
    {
        std::ranges::shuffle(*pLegalMoveOrders, HunterKillerConstants::RNG);
        auto* pOrder = pLegalMoveOrders->at(0);
        for (size_t i = 1; i < pLegalMoveOrders->size(); ++i)
        {
            delete pLegalMoveOrders->at(i);
        }
        delete pLegalMoveOrders; pLegalMoveOrders = nullptr;
        for (const auto* pLegalRotationOrder : *pLegalRotationOrders)
        {
            delete pLegalRotationOrder;
        }
        delete pLegalRotationOrders; pLegalRotationOrders = nullptr;
        for (const auto* pLegalAttackOrder : *pLegalAttackOrders)
        {
            delete pLegalAttackOrder;
        }
        delete pLegalAttackOrders; pLegalAttackOrders = nullptr;
        return pOrder;
    }
    // Do a random attack with 30% chance
    if (!pLegalAttackOrders->empty())
    {
        std::ranges::shuffle(*pLegalAttackOrders, HunterKillerConstants::RNG);
        auto* pOrder = pLegalAttackOrders->at(0);
        for (size_t i = 1; i < pLegalAttackOrders->size(); ++i)
        {
            delete pLegalAttackOrders->at(i);
        }
        delete pLegalAttackOrders; pLegalAttackOrders = nullptr;
        for (const auto* pLegalRotationOrder : *pLegalRotationOrders)
        {
            delete pLegalRotationOrder;
        }
        delete pLegalRotationOrders; pLegalRotationOrders = nullptr;
        for (const auto* pLegalMoveOrder : *pLegalMoveOrders)
        {
            delete pLegalMoveOrder;
        }
        delete pLegalMoveOrders; pLegalMoveOrders = nullptr;
        return pOrder;
    }

    return nullptr;
}
