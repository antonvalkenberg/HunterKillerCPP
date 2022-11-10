#include "HunterKillerAction.h"

HunterKillerAction::HunterKillerAction()
{
    Orders = new std::vector<HunterKillerOrder*>();
}

HunterKillerAction::HunterKillerAction(const HunterKillerState& rState)
    : ActingPlayerID(rState.GetActivePlayerID()), CurrentRound(rState.GetCurrentRound())
{
    Orders = new std::vector<HunterKillerOrder*>();
}

HunterKillerAction::~HunterKillerAction()
{
    std::ranges::for_each(*Orders, [](const HunterKillerOrder* pOrder) { delete pOrder; pOrder = nullptr; });
    delete Orders;
    Orders = nullptr;
}

// Takes ownership of the argument pointer
bool HunterKillerAction::TryAddOrder(HunterKillerOrder* pOrder) const
{
    bool success = false;
    const std::set<int>* pObjectIDs = GetObjectIDs();
    if (!pObjectIDs->contains(pOrder->GetObjectID())) {
        Orders->push_back(pOrder);
        success = true;
    }
    delete pObjectIDs;
    pObjectIDs = nullptr;
    return success;
}

bool HunterKillerAction::TryRemoveOrderForObject(const GameObject& rObject) const
{
    bool success = false;
    const std::set<int>* pObjectIDs = GetObjectIDs();
    if (pObjectIDs->contains(rObject.GetID())) {
        for (int i = 0; i < static_cast<int>(Orders->size()); i++)
        {
            if (Orders->at(i)->GetObjectID() == rObject.GetID()) {
                Orders->erase(Orders->begin() + i);
                Orders->shrink_to_fit();
                success = true;
                break;
            }
        }
    }
    delete pObjectIDs;
    pObjectIDs = nullptr;
    return success;
}

std::set<int>* HunterKillerAction::GetObjectIDs() const
{
    auto* pIDs = new std::set<int>();
    for (const HunterKillerOrder* pOrder : *Orders)
    {
        pIDs->insert(pOrder->GetObjectID());
    }
    return pIDs;
}
