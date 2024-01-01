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
    static void FilterFriendlyFire(std::vector<UnitOrder*>& rOrders, Unit* pUnit, HunterKillerMap& rMap, bool includeInfectedAllyAttacks = true, bool includeMedicAllySpecialAttacks = true);
    static void FilterFriendlyFire(std::vector<TargetedUnitOrder*>& rTargetedOrders, Unit* pUnit, HunterKillerMap& rMap, bool includeInfectedAllyAttacks = true, bool includeMedicAllySpecialAttacks = true);
    static constexpr double NO_UNIT_ORDER_THRESHOLD = 0.2;
    static constexpr double NO_BASE_ORDER_THRESHOLD = 0.1;
private:
    static bool IsTargetedOrderFriendlyFire(TargetedUnitOrder* pOrder, Unit* pUnit, const HunterKillerMap& rMap, bool includeInfectedAllyAttacks = true, bool includeMedicAllySpecialAttacks = true);
};

