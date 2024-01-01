#pragma once
#include "HunterKillerConstants.h"
#include "MapLocation.h"

class GameObject
{
public:
    GameObject() = default;
    explicit GameObject(MapLocation* pLocation) : Location(pLocation) {}
    GameObject(MapLocation* pLocation, const int maxHP) : Location(pLocation), HpMax(maxHP), HpCurrent(maxHP) {}
    GameObject(MapLocation* pLocation, const int maxHP, const int currentHP) : Location(pLocation), HpMax(maxHP), HpCurrent(currentHP) {}
    virtual ~GameObject() { Location = nullptr; }; // MapLocation memory is owned by HunterKillerMap
    virtual GameObject* Copy() = 0;
    virtual std::string ToString() = 0;
    virtual int GetHashCode() = 0;
    void ReduceHP(const int amount) { HpCurrent = std::max(HpCurrent - amount, 0); }
    void IncreaseHP(const int amount) { HpCurrent = std::min(HpCurrent + amount, HpMax); }
    bool IsDamaged() const { return HpCurrent < HpMax; }
    int GetID() const { return ID; }
    void SetID(const int id) { ID = id; }
    MapLocation& GetLocation() const { return *Location; }
    void SetLocation(MapLocation* pLocation) { Location = pLocation; }
    int GetMaxHP() const { return HpMax; }
    void SetMaxHP(const int amount) { HpMax = amount; }
    int GetCurrentHP() const { return HpCurrent; }
    void SetCurrentHP(const int amount) { HpCurrent = amount; }
private:
    int ID = -1;
    MapLocation* Location = HunterKillerConstants::GAMEOBJECT_NOT_PLACED;
    int HpMax = HunterKillerConstants::GAMEOBJECT_DEFAULT_HP;
    int HpCurrent = HunterKillerConstants::GAMEOBJECT_DEFAULT_HP;
};

