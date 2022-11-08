#pragma once
#include "Enums.h"
#include "MapFeature.h"

class Door :
    public MapFeature
{
public:
    Door() = default;
    Door(const Door& rDoor);
    explicit Door(MapLocation& rLocation) : MapFeature(rLocation, HunterKillerConstants::DOOR_DESTRUCTIBLE, HunterKillerConstants::DOOR_BLOCKS_LOS, HunterKillerConstants::DOOR_WALKABLE) {}
    Door(MapLocation& rLocation, const int timeToClose) : MapFeature(rLocation, HunterKillerConstants::DOOR_DESTRUCTIBLE, timeToClose <= 0, HunterKillerConstants::DOOR_WALKABLE), OpenTimer(timeToClose) {}
    ~Door() override = default;
    Door* Copy() override;
    std::string ToString() override { return IsOpen() ? std::format("{}", static_cast<char>(DOOR_OPEN)) : std::format("{}", static_cast<char>(DOOR_CLOSED)); }
    int GetOpenTimer() const { return OpenTimer; }
    bool IsOpen() const { return !IsBlockingLOS; }
    void Open();
    void KeepOpen();
    void Close();
    void ReduceTimer();
private:
    int OpenTimer = 0;
};

