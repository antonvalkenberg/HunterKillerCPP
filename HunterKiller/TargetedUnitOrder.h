#pragma once
#include "UnitOrder.h"

class TargetedUnitOrder final
    : public UnitOrder
{
public:
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    TargetedUnitOrder() = default; //TODO: Understand better why this is implicitly deleted (and if this should read '= delete' instead)
    TargetedUnitOrder(const int unitID, const UnitOrderType orderType, const UnitType unitType, MapLocation& rLocation) : UnitOrder(unitID, orderType, unitType), TargetLocation(rLocation) {}
    ~TargetedUnitOrder() override = default;
    [[nodiscard]] MapLocation& GetTargetLocation() const { return TargetLocation; }
    [[nodiscard]] bool IsAttackOrder() const { return OrderType == ATTACK || OrderType == ATTACK_SPECIAL; }
    static TargetedUnitOrder* MoveUnit(const Unit& rUnit, MapLocation& rAdjacentLocation);
    static TargetedUnitOrder* UnitAttack(const Unit& rUnit, MapLocation& rTargetLocation, bool useSpecialAttack);
private:
    UnitOrderType OrderType = MOVE;
    UnitType TypeOfUnit = UNIT_SOLDIER;  // NOLINT(clang-diagnostic-unused-private-field)
    MapLocation& TargetLocation;
};

