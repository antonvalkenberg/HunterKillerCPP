#include "UnitOrder.h"

UnitOrder* UnitOrder::RotateUnit(const Unit& rUnit, const bool clockwise)
{
    return new UnitOrder(rUnit.GetID(), clockwise ? ROTATE_CLOCKWISE : ROTATE_COUNTER_CLOCKWISE, rUnit.GetType());
}

UnitOrder* UnitOrder::RotateUnit(const Unit& rUnit, const Rotation rotation)
{
    return RotateUnit(rUnit, rotation == CLOCKWISE);
}