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
    [[nodiscard]] bool TryRemoveOrderForObject(const GameObject& rObject) const;
    [[nodiscard]] std::set<int>* GetObjectIDs() const;
    [[nodiscard]] int GetActingPlayerID() const { return ActingPlayerID; }
    [[nodiscard]] int GetCurrentRound() const { return CurrentRound; }
    [[nodiscard]] std::vector<HunterKillerOrder*>* GetOrders() const { return Orders; }
private:
    int ActingPlayerID = -1;
    int CurrentRound = -1;
    std::vector<HunterKillerOrder*>* Orders = nullptr;
};

