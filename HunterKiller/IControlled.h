#pragma once

class IControlled
{
public:
    virtual ~IControlled() = default;
    virtual int GetControllingPlayerID() const = 0;
    virtual bool IsControlledBy(int playerID) const = 0;
};

