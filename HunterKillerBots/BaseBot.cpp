#include "pch.h"
#include "BaseBot.h"
#include "../HunterKiller/HunterKillerMoveGenerator.h"
#include <algorithm>

UnitOrder* BaseBot::GetRandomOrder(const HunterKillerState& rState, Unit& rUnit) {
	auto* pLegalOrders = HunterKillerMoveGenerator::GetAllLegalOrders(rState, rUnit);
	
	FilterFriendlyFire(*pLegalOrders, &rUnit, rState.GetMap());
	
	if (!pLegalOrders->empty()) {
		std::ranges::shuffle(*pLegalOrders, HunterKillerConstants::RNG);
		auto* pOrder = pLegalOrders->at(0);
		for (size_t i = 1; i < pLegalOrders->size(); ++i)
		{
			delete pLegalOrders->at(i);
		}
		delete pLegalOrders; pLegalOrders = nullptr;
	    return pOrder;
	}
	
	return nullptr;
}

StructureOrder* BaseBot::GetRandomOrder(const HunterKillerState& rState, const Structure& rStructure) {
    if (auto* pLegalOrders = HunterKillerMoveGenerator::GetAllLegalOrders(rState, rStructure); !pLegalOrders->empty()) {
		std::ranges::shuffle(*pLegalOrders, HunterKillerConstants::RNG);
		auto* pOrder = pLegalOrders->at(0);
		for (size_t i = 1; i < pLegalOrders->size(); ++i)
		{
			delete pLegalOrders->at(i);
		}
		delete pLegalOrders; pLegalOrders = nullptr;
	    return pOrder;
	}
	
	return nullptr;
}

void BaseBot::FilterFriendlyFire(std::vector<UnitOrder*>& rOrders, Unit* pUnit, HunterKillerMap& rMap) {

	const auto removableOrders = std::ranges::remove_if(rOrders.begin(), rOrders.end(), [pUnit, rMap](const UnitOrder* pOrder)
	{
		// Skip non-attack orders
		if (!pOrder->IsAttackOrder())
			return false;
		// Remove all attacks without a proper target
		if (rMap.IsAttackOrderWithoutTarget(*pOrder))
		{
			delete pOrder;
			return true;
		}
		// Remove all attacks with our own location as target
		if (pOrder->GetTargetLocation().has_value() && pOrder->GetTargetLocation().value() == pUnit->GetLocation())
		{
			delete pOrder;
			return true;
		}
		// Remove all attack with an ally structure as target
		if (rMap.IsAttackOrderTargetingAllyStructure(*pOrder, pUnit))
		{
			delete pOrder;
			return true;
		}
		// Remove all attacks with an ally unit as target
		if (rMap.IsAttackOrderTargetingAllyUnit(*pOrder, pUnit))
		{
		    // Unless the order is a for an Infected, or a Medic's special attack
			const bool goodSelfOrder = pUnit->GetType() == UNIT_INFECTED || (pUnit->GetType() == UNIT_MEDIC && pOrder->GetOrderType() == ATTACK_SPECIAL);
            if (!goodSelfOrder)
			{
				delete pOrder;
				return true;
			}
		}
		// Other orders are OK
		return false;
	});

	rOrders.erase(removableOrders.begin(), removableOrders.end());
}