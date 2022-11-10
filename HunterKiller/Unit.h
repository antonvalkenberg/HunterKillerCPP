#pragma once
#include <unordered_set>

#include "GameObject.h"
#include "HunterKillerHash.h"
#include "IControlled.h"

class Unit
    : public GameObject, public IControlled {
public:
    Unit() = default;
    Unit(int SpawningPlayerID, UnitType unitType, MapLocation& rLocation, int MaxHP, int CurrentHP, Direction Facing, int FoVRange, int FoVAngle, int attackRange, int attackDamage, int Cooldown, int Cost, int Score);
    ~Unit() override;
    virtual void StartCooldown() = 0;
    Unit* Copy() override = 0;
    std::string ToString() override = 0;
    int GetControllingPlayerID() const override;
    bool IsControlledBy(int playerID) const override;
    void SetSpecialAttackCooldown(const int cooldownRemaining) { SpecialAttackCooldown = cooldownRemaining; }
    void ReduceCooldown() { if (SpecialAttackCooldown > 0) SpecialAttackCooldown--; }
    bool IsFieldOfViewValid() const { return FieldOfViewValid; }
    bool IsInFieldOfView(const MapLocation& rLocation) const { return (FieldOfView && FieldOfView->contains(rLocation)); }
    void UpdateFieldOfView(std::unordered_set<MapLocation, MapLocationHash>* pFieldOfView) { delete FieldOfView; FieldOfView = pFieldOfView; FieldOfViewValid = true; }
    void InvalidateFieldOfView() { delete FieldOfView; FieldOfView = nullptr; FieldOfViewValid = false; }
    static int GetAttackRange(UnitType unitType);
    static int GetSpawnCost(UnitType unitType);
    bool IsWithinRange(const MapLocation& rLocation, const int range) const { return MapLocation::GetManhattanDistance(this->GetLocation(), rLocation) <= range; }
    bool CanUseSpecialAttack() const { return SpecialAttackCooldown == 0; }
    UnitType GetType() const { return TypeOfUnit; }
    Direction GetOrientation() const { return Orientation; }
    void SetOrientation(const Direction orientation) { Orientation = orientation; }
    int GetFieldOfViewRange() const { return FieldOfViewRange; }
    int GetFieldOfViewAngle() const { return FieldOfViewAngle; }
    int GetAttackRange() const { return AttackRange; }
    int GetAttackDamage() const { return AttackDamage; }
    int GetSpecialAttackCooldown() const { return SpecialAttackCooldown; }
    int GetSpawnCost() const { return SpawnCost; }
    int GetScoreWorth() const { return ScoreWorth; }
    [[nodiscard]] std::unordered_set<MapLocation, MapLocationHash>* GetFieldOfView() const { return FieldOfView; }
private:
    int ControllingPlayerID = -1;
    UnitType TypeOfUnit = UNIT_SOLDIER;
    Direction Orientation = HunterKillerConstants::UNIT_DEFAULT_ORIENTATION;
    int FieldOfViewRange = HunterKillerConstants::UNIT_DEFAULT_FOV_RANGE;
    int FieldOfViewAngle = HunterKillerConstants::UNIT_DEFAULT_FOV_ANGLE;
    int AttackRange = HunterKillerConstants::UNIT_DEFAULT_ATTACK_RANGE;
    int AttackDamage = HunterKillerConstants::UNIT_DEFAULT_ATTACK_DAMAGE;
    int SpecialAttackCooldown = HunterKillerConstants::UNIT_DEFAULT_SPECIAL_COOLDOWN;
    int SpawnCost = HunterKillerConstants::UNIT_DEFAULT_SPAWN_COST;
    int ScoreWorth = HunterKillerConstants::UNIT_DEFAULT_SCORE;
    bool FieldOfViewValid = false;
protected:
    std::unordered_set<MapLocation, MapLocationHash>* FieldOfView = nullptr;
};
