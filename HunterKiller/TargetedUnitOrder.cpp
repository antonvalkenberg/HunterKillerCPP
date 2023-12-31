#include "TargetedUnitOrder.h"

TargetedUnitOrder* TargetedUnitOrder::MoveUnit(const Unit& rUnit, MapLocation& rAdjacentLocation)
{
    return new TargetedUnitOrder(rUnit.GetID(), MOVE, rUnit.GetType(), rAdjacentLocation);
}

TargetedUnitOrder* TargetedUnitOrder::UnitAttack(const Unit& rUnit, MapLocation& rTargetLocation, const bool useSpecialAttack)
{
    return new TargetedUnitOrder(rUnit.GetID(), useSpecialAttack ? ATTACK_SPECIAL : ATTACK, rUnit.GetType(), rTargetLocation);
}