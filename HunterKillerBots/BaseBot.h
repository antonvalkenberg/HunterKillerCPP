#pragma once
#include "../HunterKiller/HunterKillerAction.h"
#include "../HunterKiller/StructureOrder.h"

class BaseBot
{
public:
    BaseBot() = default;
    virtual ~BaseBot() = default;
    virtual HunterKillerAction* Handle(const HunterKillerState& rState) = 0;
    [[nodiscard]] virtual const std::string* GetBotName() const = 0;
    static UnitOrder* GetRandomOrder(const HunterKillerState& rState, Unit& rUnit);
    static StructureOrder* GetRandomOrder(const HunterKillerState& rState, const Structure& rStructure);
    static void FilterFriendlyFire(std::vector<UnitOrder*>& rOrders, Unit* pUnit, HunterKillerMap& rMap);
};

