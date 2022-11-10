#include "pch.h"
#include "QuickRandomBot.h"
#include "../HunterKiller/HunterKillerMoveGenerator.h"

QuickRandomBot::~QuickRandomBot()
{
    delete BotName; BotName = nullptr;
}

HunterKillerAction* QuickRandomBot::Handle(const HunterKillerState& rState)
{
    auto* pRandomAction = new HunterKillerAction(rState);
    const auto& rPlayer = rState.GetActivePlayer();
    const auto& rMap = rState.GetMap();
    const auto* pStructures = rPlayer.GetStructures(rMap);
    const auto* pUnits = rPlayer.GetUnits(rMap);

    for (const auto* pStructure : *pStructures)
    {
        if (StructureOrder* pOrder = HunterKillerMoveGenerator::GetRandomOrder(rState, *pStructure))
            pRandomAction->TryAddOrder(pOrder);
    }
    for (auto* pUnit : *pUnits)
    {
        if (UnitOrder* pOrder = HunterKillerMoveGenerator::GetRandomOrder(rState, *pUnit))
            pRandomAction->TryAddOrder(pOrder);
    }

    delete pStructures; pStructures = nullptr;
    delete pUnits; pUnits = nullptr;
    return pRandomAction;
}
