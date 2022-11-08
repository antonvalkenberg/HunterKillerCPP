#pragma once
#include <set>

#include "GameObject.h"
#include "HunterKillerOrder.h"
#include "HunterKillerState.h"

class HunterKillerAction
{
public:
    HunterKillerAction();
    explicit HunterKillerAction(const HunterKillerState& rState);
    ~HunterKillerAction();
    bool TryAddOrder(HunterKillerOrder* pOrder) const;
    bool TryRemoveOrderForObject(const GameObject& rObject) const;
    [[nodiscard]] std::set<int>* GetObjectIDs() const;
    int GetActingPlayerID() const { return ActingPlayerID; }
    int GetCurrentRound() const { return CurrentRound; }
    [[nodiscard]] std::vector<HunterKillerOrder*>* GetOrders() const { return Orders; }
private:
    int ActingPlayerID = -1;
    int CurrentRound = -1;
    std::vector<HunterKillerOrder*>* Orders = nullptr;
};

