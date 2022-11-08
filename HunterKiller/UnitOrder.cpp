#include "UnitOrder.h"

UnitOrder* UnitOrder::RotateUnit(const Unit& rUnit, const bool clockwise)
{
    return new UnitOrder(rUnit.GetID(), clockwise ? ROTATE_CLOCKWISE : ROTATE_COUNTER_CLOCKWISE, rUnit.GetType());
}

UnitOrder* UnitOrder::RotateUnit(const Unit& rUnit, const Rotation rotation)
{
    return RotateUnit(rUnit, rotation == CLOCKWISE);
}

UnitOrder* UnitOrder::MoveUnit(const Unit& rUnit, const MapLocation& rAdjacentLocation)
{
    return new UnitOrder(rUnit.GetID(), MOVE, rUnit.GetType(), rAdjacentLocation);
}

UnitOrder* UnitOrder::UnitAttack(const Unit& rUnit, const MapLocation& rTargetLocation, const bool useSpecialAttack)
{
    return new UnitOrder(rUnit.GetID(), useSpecialAttack ? ATTACK_SPECIAL : ATTACK, rUnit.GetType(), rTargetLocation);
}