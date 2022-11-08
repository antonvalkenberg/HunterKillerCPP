#include "Door.h"

Door::Door(const Door& rDoor) : Door(rDoor.GetLocation(), rDoor.GetOpenTimer())
{
    SetID(rDoor.GetID());
}

Door* Door::Copy()
{
    return new Door(*this);
}

void Door::Open()
{
    OpenTimer = HunterKillerConstants::DOOR_OPEN_ROUNDS;
    IsBlockingLOS = false;
}

void Door::KeepOpen()
{
    OpenTimer = 1;
    IsBlockingLOS = false;
}

void Door::Close()
{
    OpenTimer = 0;
    IsBlockingLOS = true;
}

void Door::ReduceTimer()
{
    if (OpenTimer > 0)
        OpenTimer--;
}
