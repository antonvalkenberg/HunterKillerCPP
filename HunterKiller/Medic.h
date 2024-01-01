#pragma once
#include "Unit.h"

class Medic
    : public Unit
{
public:
    Medic() = default;
    Medic(const Medic& rMedic);
    Medic(int spawningPlayerID, MapLocation& rLocation, Direction facing);
    Medic(int spawningPlayerID, MapLocation& rLocation, int maxHP, int currentHP, Direction facing, int fovRange, int fovAngle, int attackRange, int attackDamage, int cooldown, int cost, int score);
    ~Medic() override { delete FieldOfView; FieldOfView = nullptr; }
    Medic* Copy() override { return new Medic(*this); }
    int GetHashCode() override { return 43 ^ GetID() ^ GetControllingPlayerID() ^ UNIT_MEDIC ^ GetLocation().GetHashCode() ^ GetOrientation(); }
    void StartCooldown() override { SetSpecialAttackCooldown(HunterKillerConstants::MEDIC_COOLDOWN); }
    std::string ToString() override { return std::format("{}", static_cast<char>(MEDIC)); }
    std::string ToStringInformational() override { return std::format("Medic, {0} HP, {1}", GetCurrentHP(), CanUseSpecialAttack() ? "Heal ready" : std::format("Heal available in {0} round(s)", GetSpecialAttackCooldown())); }
};

