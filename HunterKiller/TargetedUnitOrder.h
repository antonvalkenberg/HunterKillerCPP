#pragma once
#include "UnitOrder.h"

class TargetedUnitOrder
    : public UnitOrder
{
public:
    TargetedUnitOrder() = default;
    TargetedUnitOrder(const int unitID, const UnitOrderType orderType, const UnitType unitType, MapLocation& rLocation) : UnitOrder(unitID, orderType, unitType), TargetLocation(rLocation) {}
    ~TargetedUnitOrder() override = default;
    [[nodiscard]] MapLocation& GetTargetLocation() { return TargetLocation; }
    bool IsAttackOrder() const { return OrderType == ATTACK || OrderType == ATTACK_SPECIAL; }
    static TargetedUnitOrder* MoveUnit(const Unit& rUnit, MapLocation& rAdjacentLocation);
    static TargetedUnitOrder* UnitAttack(const Unit& rUnit, MapLocation& rTargetLocation, bool useSpecialAttack);
private:
    UnitOrderType OrderType = MOVE;
    UnitType TypeOfUnit = UNIT_SOLDIER;
    MapLocation& TargetLocation;
};

