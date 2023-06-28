#pragma once
#include <optional>

#include "Enums.h"
#include "HunterKillerOrder.h"
#include "Unit.h"

class UnitOrder
    : public HunterKillerOrder
{
public:
    UnitOrder() = default;
    UnitOrder(const int unitID, const UnitOrderType orderType, const UnitType unitType) : HunterKillerOrder(unitID), OrderType(orderType), TypeOfUnit(unitType), TargetLocation(std::optional<MapLocation>()) {}
    UnitOrder(const int unitID, const UnitOrderType orderType, const UnitType unitType, const MapLocation& rLocation) : HunterKillerOrder(unitID), OrderType(orderType), TypeOfUnit(unitType), TargetLocation(rLocation) {}
    ~UnitOrder() override = default;
    [[nodiscard]] std::optional<MapLocation> GetTargetLocation() const { return TargetLocation; }
    UnitOrderType GetOrderType() const { return OrderType; }
    UnitType GetUnitType() const { return TypeOfUnit; }
    bool IsAttackOrder() const { return OrderType == ATTACK || OrderType == ATTACK_SPECIAL; }
    static UnitOrder* RotateUnit(const Unit& rUnit, bool clockwise);
    static UnitOrder* RotateUnit(const Unit& rUnit, Rotation rotation);
    static UnitOrder* MoveUnit(const Unit& rUnit, const MapLocation& rAdjacentLocation);
    static UnitOrder* UnitAttack(const Unit& rUnit, const MapLocation& rTargetLocation, bool useSpecialAttack);
private:
    UnitOrderType OrderType = MOVE;
    UnitType TypeOfUnit = UNIT_SOLDIER;
    std::optional<MapLocation> TargetLocation;
};

