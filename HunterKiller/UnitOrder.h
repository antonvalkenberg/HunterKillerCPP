#pragma once
#include "Enums.h"
#include "HunterKillerOrder.h"
#include "Unit.h"

class UnitOrder
    : public HunterKillerOrder
{
public:
    UnitOrder() = default;
    UnitOrder(const int unitID, const UnitOrderType orderType, const UnitType unitType) : HunterKillerOrder(unitID), OrderType(orderType), TypeOfUnit(unitType) {}
    ~UnitOrder() override = default;
    UnitOrderType GetOrderType() const { return OrderType; }
    UnitType GetUnitType() const { return TypeOfUnit; }
    static UnitOrder* RotateUnit(const Unit& rUnit, bool clockwise);
    static UnitOrder* RotateUnit(const Unit& rUnit, Rotation rotation);
private:
    UnitOrderType OrderType = MOVE;
    UnitType TypeOfUnit = UNIT_SOLDIER;
};

