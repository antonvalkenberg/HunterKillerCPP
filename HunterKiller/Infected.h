#pragma once
#include "Unit.h"

class Infected
    : public Unit
{
public:
    Infected() = default;
    Infected(const Infected& rInfected);
    Infected(int spawningPlayerID, MapLocation& rLocation, Direction facing);
    Infected(int spawningPlayerID, MapLocation& rLocation, int maxHP, int currentHP, Direction facing, int fovRange, int fovAngle, int attackRange, int attackDamage, int cooldown, int cost, int score);
    ~Infected() override { delete FieldOfView; FieldOfView = nullptr; }
    Infected* Copy() override { return new Infected(*this); }
    int GetHashCode() override { return 47 ^ GetID() ^ GetControllingPlayerID() ^ UNIT_INFECTED ^ GetLocation().GetHashCode() ^ GetOrientation(); }
    void StartCooldown() override { SetSpecialAttackCooldown(HunterKillerConstants::INFECTED_COOLDOWN); }
    std::string ToString() override { return std::format("{}", static_cast<char>(INFECTED)); }
};

