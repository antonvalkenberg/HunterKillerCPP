#include "Infected.h"

Infected::Infected(const Infected& rInfected)
    : Infected(rInfected.GetControllingPlayerID(), rInfected.GetLocation(), rInfected.GetMaxHP(), rInfected.GetCurrentHP(), rInfected.GetOrientation(),
        rInfected.GetFieldOfViewRange(), rInfected.GetFieldOfViewAngle(),
        rInfected.GetAttackRange(), rInfected.GetAttackDamage(), rInfected.GetSpecialAttackCooldown(),
        rInfected.GetSpawnCost(), rInfected.GetScoreWorth())
{
    SetID(rInfected.GetID());
    UpdateFieldOfView(new std::unordered_set(*rInfected.GetFieldOfView()));
}

Infected::Infected(const int spawningPlayerID, MapLocation& rLocation, const Direction facing)
    : Infected(spawningPlayerID, rLocation, HunterKillerConstants::INFECTED_MAX_HP, HunterKillerConstants::INFECTED_MAX_HP, facing,
        HunterKillerConstants::INFECTED_FOV_RANGE, HunterKillerConstants::INFECTED_FOV_ANGLE,
        HunterKillerConstants::INFECTED_ATTACK_RANGE, HunterKillerConstants::INFECTED_ATTACK_DAMAGE, 0,
        HunterKillerConstants::INFECTED_SPAWN_COST, HunterKillerConstants::INFECTED_SCORE)
{
}

Infected::Infected(const int spawningPlayerID, MapLocation& rLocation, const int maxHP, const int currentHP, const Direction facing, const int fovRange, const int fovAngle, const int attackRange, const int attackDamage, const int cooldown, const int cost, const int score)
    : Unit(spawningPlayerID, UNIT_INFECTED, rLocation, maxHP, currentHP, facing, fovRange, fovAngle, attackRange, attackDamage, cooldown, cost, score)
{
}