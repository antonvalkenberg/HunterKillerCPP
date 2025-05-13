#pragma once

class IControlled
{
public:
    virtual ~IControlled() = default;
    [[nodiscard]] virtual int GetControllingPlayerID() const = 0;
    [[nodiscard]] virtual bool IsControlledBy(int playerID) const = 0;
};

