// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#pragma once
#include "Enums.h"
#include "HunterKillerOrder.h"
#include "Structure.h"

class StructureOrder
    : public HunterKillerOrder {
public:
    StructureOrder() = default;
    StructureOrder(const int structureID, const StructureOrderType type) : HunterKillerOrder(structureID), OrderType(type) {}
    ~StructureOrder() override = default;
    [[nodiscard]] std::string ToString() const { return std::format("Structure ({0}) | {1}", GetObjectID(), static_cast<int>(OrderType)); }
    StructureOrderType GetOrderType() const { return OrderType; }
    static StructureOrder* Spawn(const Structure& rStructure, UnitType unitType);
private:
    StructureOrderType OrderType = SPAWN_SOLDIER;
};

inline StructureOrder* StructureOrder::Spawn(const Structure& rStructure, const UnitType unitType)
{
    switch (unitType)
    {
    case UNIT_SOLDIER: return new StructureOrder(rStructure.GetID(), SPAWN_SOLDIER);
    case UNIT_MEDIC: return new StructureOrder(rStructure.GetID(), SPAWN_MEDIC);
    case UNIT_INFECTED: return new StructureOrder(rStructure.GetID(), SPAWN_INFECTED);
    default: return nullptr;
    }
}
