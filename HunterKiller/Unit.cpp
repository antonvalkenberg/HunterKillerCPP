// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "Unit.h"

Unit::Unit(const int SpawningPlayerID, const UnitType unitType, MapLocation& rLocation, const int MaxHP, const int CurrentHP, const Direction Facing, const int FoVRange, const int FoVAngle, const int attackRange, const int attackDamage, const int Cooldown, const int Cost, const int Score)
    : GameObject(&rLocation, MaxHP, CurrentHP), ControllingPlayerID(SpawningPlayerID), TypeOfUnit(unitType)
{
    Orientation = Facing;
    FieldOfViewRange = FoVRange;
    FieldOfViewAngle = FoVAngle;
    AttackRange = attackRange;
    AttackDamage = attackDamage;
    SpecialAttackCooldown = Cooldown;
    SpawnCost = Cost;
    ScoreWorth = Score;

    FieldOfView = new std::unordered_set<MapLocation, MapLocationHash>();
    FieldOfViewValid = false;
}

Unit::~Unit() {
    delete FieldOfView;
    FieldOfView = nullptr;
}

int Unit::GetControllingPlayerID() const
{
    return ControllingPlayerID;
}

bool Unit::IsControlledBy(const int playerID) const
{
    return ControllingPlayerID == playerID;
}

int Unit::GetAttackRange(const UnitType unitType) {
    switch (unitType)
    {
    case UNIT_INFECTED:
        return HunterKillerConstants::INFECTED_ATTACK_RANGE;
    case UNIT_MEDIC:
        return HunterKillerConstants::MEDIC_ATTACK_RANGE;
    case UNIT_SOLDIER:
        return HunterKillerConstants::SOLDIER_ATTACK_RANGE;
    default:
        return 0;
    }
}

int Unit::GetSpawnCost(const UnitType unitType) {
    switch (unitType)
    {
    case UNIT_INFECTED:
        return HunterKillerConstants::INFECTED_SPAWN_COST;
    case UNIT_MEDIC:
        return HunterKillerConstants::MEDIC_SPAWN_COST;
    case UNIT_SOLDIER:
        return HunterKillerConstants::SOLDIER_SPAWN_COST;
    default:
        return 0;
    }
}