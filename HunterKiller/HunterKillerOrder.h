#pragma once

class HunterKillerOrder
{
public:
    HunterKillerOrder() = default;
    explicit HunterKillerOrder(const int gameObjectID) : ObjectID(gameObjectID) {}
    virtual ~HunterKillerOrder() = default;
    int GetObjectID() const { return ObjectID; }
    bool IsAccepted() const { return Accepted; }
    void SetAccepted(const bool accepted) { Accepted = accepted; }
private:
    int ObjectID = -1;
    bool Accepted = false;
};

