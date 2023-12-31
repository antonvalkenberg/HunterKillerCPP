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

void BaseBot::FilterFriendlyFire(std::vector<UnitOrder*>& rOrders, Unit* pUnit, HunterKillerMap& rMap, bool includeInfectedAllyAttacks, bool includeMedicAllySpecialAttacks) {

	const auto removableOrders = std::ranges::remove_if(rOrders.begin(), rOrders.end(), [pUnit, rMap, includeInfectedAllyAttacks, includeMedicAllySpecialAttacks](UnitOrder* pOrder)
	{
		auto* pTargetedUnitOrder = dynamic_cast<TargetedUnitOrder*>(pOrder);
		// Skip non-attack orders
		if (!pTargetedUnitOrder || !pTargetedUnitOrder->IsAttackOrder())
			return false;
		if (IsTargetedOrderFriendlyFire(pTargetedUnitOrder, pUnit, rMap, includeInfectedAllyAttacks, includeMedicAllySpecialAttacks)) {
			delete pTargetedUnitOrder;
			return true;
		}
		return false;
	});

	rOrders.erase(removableOrders.begin(), removableOrders.end());
}

void BaseBot::FilterFriendlyFire(std::vector<TargetedUnitOrder*>& rTargetedOrders, Unit* pUnit, HunterKillerMap& rMap, bool includeInfectedAllyAttacks, bool includeMedicAllySpecialAttacks) {

	const auto removableOrders = std::ranges::remove_if(rTargetedOrders.begin(), rTargetedOrders.end(), [pUnit, rMap, includeInfectedAllyAttacks, includeMedicAllySpecialAttacks](TargetedUnitOrder* pTargetedOrder)
	{
		if (IsTargetedOrderFriendlyFire(pTargetedOrder, pUnit, rMap, includeInfectedAllyAttacks, includeMedicAllySpecialAttacks)) {
			delete pTargetedOrder;
			return true;
		}
		return false;
	});

	rTargetedOrders.erase(removableOrders.begin(), removableOrders.end());
}

bool BaseBot::IsTargetedOrderFriendlyFire(TargetedUnitOrder* pOrder, Unit* pUnit, const HunterKillerMap& rMap, bool includeInfectedAllyAttacks, bool includeMedicAllySpecialAttacks)
{
	// Remove all attacks with our own location as target
	if (pOrder->GetTargetLocation() == pUnit->GetLocation())
		return true;
	// Remove all attack with an ally structure as target
	if (rMap.IsAttackOrderTargetingAllyStructure(*pOrder, pUnit))
		return true;
	// Remove all attacks with an ally unit as target
	if (rMap.IsAttackOrderTargetingAllyUnit(*pOrder, pUnit))
	{
		// Unless the order is a for an Infected, or a Medic's special attack
		const bool goodSelfOrder = (includeInfectedAllyAttacks && pUnit->GetType() == UNIT_INFECTED) ||	(includeMedicAllySpecialAttacks && pUnit->GetType() == UNIT_MEDIC && pOrder->GetOrderType() == ATTACK_SPECIAL);
        return !goodSelfOrder;
	}
	// Other orders are OK
	return false;
}
