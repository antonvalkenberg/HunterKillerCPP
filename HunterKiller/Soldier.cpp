#include "Soldier.h"

Soldier::Soldier(const Soldier& rSoldier)
    : Soldier(rSoldier.GetControllingPlayerID(), rSoldier.GetLocation(), rSoldier.GetMaxHP(), rSoldier.GetCurrentHP(), rSoldier.GetOrientation(),
        rSoldier.GetFieldOfViewRange(), rSoldier.GetFieldOfViewAngle(),
        rSoldier.GetAttackRange(), rSoldier.GetAttackDamage(), rSoldier.GetSpecialAttackCooldown(),
        rSoldier.GetSpawnCost(), rSoldier.GetScoreWorth())
{
    SetID(rSoldier.GetID());
    UpdateFieldOfView(new std::unordered_set(*rSoldier.GetFieldOfView()));
}

Soldier::Soldier(const int spawningPlayerID, MapLocation& rLocation, const Direction facing)
    : Soldier(spawningPlayerID, rLocation, HunterKillerConstants::SOLDIER_MAX_HP, HunterKillerConstants::SOLDIER_MAX_HP, facing,
        HunterKillerConstants::SOLDIER_FOV_RANGE, HunterKillerConstants::SOLDIER_FOV_ANGLE,
        HunterKillerConstants::SOLDIER_ATTACK_RANGE, HunterKillerConstants::SOLDIER_ATTACK_DAMAGE, 0,
        HunterKillerConstants::SOLDIER_SPAWN_COST, HunterKillerConstants::SOLDIER_SCORE)
{
}

Soldier::Soldier(const int spawningPlayerID, MapLocation& rLocation, const int maxHP, const int currentHP, const Direction facing, const int fovRange, const int fovAngle, const int attackRange, const int attackDamage, const int cooldown, const int cost, const int score)
    : Unit(spawningPlayerID, UNIT_SOLDIER, rLocation, maxHP, currentHP, facing, fovRange, fovAngle, attackRange, attackDamage, cooldown, cost, score)
{
}