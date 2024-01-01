#pragma once
#include "Unit.h"

class Soldier
    : public Unit
{
public:
    Soldier() = default;
    Soldier(const Soldier& rSoldier);
    Soldier(int spawningPlayerID, MapLocation& rLocation, Direction facing);
    Soldier(int spawningPlayerID, MapLocation& rLocation, int maxHP, int currentHP, Direction facing, int fovRange, int fovAngle, int attackRange, int attackDamage, int cooldown, int cost, int score);
    ~Soldier() override { delete FieldOfView; FieldOfView = nullptr; }
    Soldier* Copy() override { return new Soldier(*this); }
    int GetHashCode() override { return 41 ^ GetID() ^ GetControllingPlayerID() ^ UNIT_SOLDIER ^ GetLocation().GetHashCode() ^ GetOrientation(); }
    void StartCooldown() override { SetSpecialAttackCooldown(HunterKillerConstants::SOLDIER_COOLDOWN); }
    std::string ToString() override { return std::format("{}", static_cast<char>(SOLDIER)); }
    std::string ToStringInformational() override { return std::format("Soldier, {0} HP, {1}", GetCurrentHP(), CanUseSpecialAttack() ? "Grenade ready" : std::format("Grenade available in {0} round(s)", GetSpecialAttackCooldown())); }
};

