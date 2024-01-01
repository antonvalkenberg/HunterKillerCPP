#include "Medic.h"

Medic::Medic(const Medic& rMedic)
    : Medic(rMedic.GetControllingPlayerID(), rMedic.GetLocation(), rMedic.GetMaxHP(), rMedic.GetCurrentHP(), rMedic.GetOrientation(),
        rMedic.GetFieldOfViewRange(), rMedic.GetFieldOfViewAngle(),
        rMedic.GetAttackRange(), rMedic.GetAttackDamage(), rMedic.GetSpecialAttackCooldown(),
        rMedic.GetSpawnCost(), rMedic.GetScoreWorth())
{
    SetID(rMedic.GetID());
    UpdateFieldOfView(new std::unordered_set(*rMedic.GetFieldOfView()));
}

Medic::Medic(const int spawningPlayerID, MapLocation& rLocation, const Direction facing)
    : Medic(spawningPlayerID, rLocation, HunterKillerConstants::MEDIC_MAX_HP, HunterKillerConstants::MEDIC_MAX_HP, facing,
        HunterKillerConstants::MEDIC_FOV_RANGE, HunterKillerConstants::MEDIC_FOV_ANGLE,
        HunterKillerConstants::MEDIC_ATTACK_RANGE, HunterKillerConstants::MEDIC_ATTACK_DAMAGE, 0,
        HunterKillerConstants::MEDIC_SPAWN_COST, HunterKillerConstants::MEDIC_SCORE)
{
}

Medic::Medic(const int spawningPlayerID, MapLocation& rLocation, const int maxHP, const int currentHP, const Direction facing, const int fovRange, const int fovAngle, const int attackRange, const int attackDamage, const int cooldown, const int cost, const int score)
    : Unit(spawningPlayerID, UNIT_MEDIC, rLocation, maxHP, currentHP, facing, fovRange, fovAngle, attackRange, attackDamage, cooldown, cost, score)
{
}