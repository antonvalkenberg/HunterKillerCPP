#pragma once
#include "HunterKillerState.h"
#include "StructureOrder.h"

class HunterKillerMoveGenerator
{
public:
    static std::vector<StructureOrder*>* GetAllLegalOrders(const HunterKillerState& rState, const Structure& rStructure);
    static StructureOrder* GetRandomOrder(const HunterKillerState& rState, const Structure& rStructure);
    static UnitOrder* GetRandomOrder(const HunterKillerState& rState, Unit& rUnit);
    static std::vector<UnitOrder*>* GetAllLegalOrders(const HunterKillerState& rState, Unit& rUnit) { return GetAllLegalOrders(rState, rUnit, true, true, true); }
    static std::vector<UnitOrder*>* GetAllLegalOrders(const HunterKillerState& rState, Unit& rUnit, bool includeRotation, bool includeMovement, bool includeAttack);
    static std::vector<TargetedUnitOrder*>* GetAllLegalMoveOrders(const HunterKillerState& rState, const Unit& rUnit);
    static TargetedUnitOrder* GetRandomMoveOrder(const HunterKillerState& rState, const Unit& rUnit);
    static std::vector<UnitOrder*>* GetAllLegalRotationOrders(const Unit& rUnit);
    static std::vector<TargetedUnitOrder*>* GetAllLegalAttackOrders(const HunterKillerState& rState, Unit& rUnit) { return GetAllLegalAttackOrders(rState, rUnit, false); }
    static std::vector<TargetedUnitOrder*>* GetAllLegalAttackOrders(const HunterKillerState& rState, Unit& rUnit, bool usePlayersFoV);
    static TargetedUnitOrder* GetRandomAttackOrder(const HunterKillerState& rState, Unit& rUnit, bool usePlayersFoV, bool useSpecial);
};

