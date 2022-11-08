// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
// ReSharper disable StringLiteralTypo
#pragma once
#include <array>
#include <format>
#include <optional>
#include <stdexcept>

#include "Enums.h"

class EnumExtensions
{
public:

    static Direction TryRotate(const Direction facing, const Rotation rotation)
    {
        switch (facing)
        {
        case NORTH: return (rotation == CLOCKWISE) ? EAST : WEST;
        case EAST: return (rotation == CLOCKWISE) ? SOUTH : NORTH;
        case SOUTH: return (rotation == CLOCKWISE) ? WEST : EAST;
        case WEST: return (rotation == CLOCKWISE) ? NORTH : SOUTH;
        default:
            throw std::invalid_argument(std::format("Unsupported Direction provided when rotating: '{}'\n", (int)facing));
        }
    }
    static Direction TryGetOppositeDirection(const Direction direction) {
        switch (direction)
        {
        case NORTH: return SOUTH;
        case EAST: return WEST;
        case SOUTH: return NORTH;
        case WEST: return EAST;
        default:
            throw std::invalid_argument(std::format("Unsupported Direction provided when getting opposite direction: '{}'\n", (int)direction));
        }
    }
    static Direction TryParseDirection(const char dir)
    {
        switch (dir)
        {
        case 'N': return NORTH;
        case 'E': return EAST;
        case 'S': return SOUTH;
        case 'W': return WEST;
            default:
                throw std::invalid_argument(std::format("Could not parse provided argument as a Direction: '{}'\n", dir));
        }
    }
    static std::optional<Rotation> GetRotationRequiredToFace(const Direction facing, const Direction target) {
        if (target == facing) return {};
        if (target == TryGetOppositeDirection(facing)) return { CLOCKWISE };

        switch (facing)
        {
        case NORTH: return target == WEST ? std::optional(COUNTER_CLOCKWISE) : std::optional(CLOCKWISE);
        case EAST: return target == NORTH ? std::optional(COUNTER_CLOCKWISE) : std::optional(CLOCKWISE);
        case SOUTH: return target == EAST ? std::optional(COUNTER_CLOCKWISE) : std::optional(CLOCKWISE);
        case WEST: return target == SOUTH ? std::optional(COUNTER_CLOCKWISE) : std::optional(CLOCKWISE);
        default:
            return {};
        }
    }
    static TileType TryParse(char c) {
        switch (c)
        {
        case 'H': return STRONGHOLD;
        case 'P': return OUTPOST;
        case 'X': return OBJECTIVE;
        case 'B': return BASE;
        case 'I': return INFECTED;
        case 'M': return MEDIC;
        case 'S': return SOLDIER;
        case 'O': return DOOR_OPEN;
        case 'D': return DOOR_CLOSED;
        case '|': return WALL;
        case '_': return FLOOR;
        case '.': return SPACE;
        default:
            throw std::invalid_argument(std::format("Could not parse provided argument as a TileType: '{}'\n", c));
        }
    }
    static bool NeedsLocation(const UnitOrderType unitOrderType) {
        switch (unitOrderType)
        {
        case MOVE:
        case ATTACK:
        case ATTACK_SPECIAL:
            return true;
        case ROTATE_CLOCKWISE:
        case ROTATE_COUNTER_CLOCKWISE:
        default:
            return false;
        }
    }
    static FourPatchSections GetSectionFamily(const FourPatchSections section)
    {
        switch (section)
        {
        case A:
        case A_HM:
        case A_VM:
        case A_M: return A;
        case B:
        case B_HM: return B;
        case C:
        case C_VM: return C;
        case D: return D;
        }
        return {};
    }
    static bool IsHorizontalMirror(const FourPatchSections section)
    {
        switch (section)
        {
        case A_HM:
        case B_HM:
        case A_M: return true;
        case A:
        case A_VM:
        case B:
        case C:
        case C_VM:
        case D: return false;
        }
        return {};
    }
    static bool IsVerticalMirror(const FourPatchSections section)
    {
        switch (section)
        {
        case A_VM:
        case C_VM:
        case A_M: return true;
        case A:
        case A_HM:
        case B:
        case B_HM:
        case C:
        case D: return false;
        }
        return {};
    }
    static bool IsFullMirror(const FourPatchSections section)
    {
        switch (section)
        {
        case A_M: return true;
        case A:
        case A_HM:
        case A_VM:
        case B:
        case B_HM:
        case C:
        case C_VM:
        case D: return false;
        }
        return {};
    }

    static std::array<Direction, 4> GetDirections() { return { NORTH, EAST, SOUTH, WEST }; }
    static std::array<Rotation, 2> GetRotations() { return { CLOCKWISE, COUNTER_CLOCKWISE }; }
    static std::array<GameMode, 5> GetGameModes() { return { CAPTURE, KILLING, KING_OF_THE_HILL, EXPLORATION, DEATHMATCH }; }
    static std::array<MapType, 3> GetMapTypes() { return { DEFAULT, OPEN, NARROW }; }
    static std::array<StructureOrderType, 3> GetStructureOrderTypes() { return { SPAWN_INFECTED, SPAWN_MEDIC, SPAWN_SOLDIER }; }
    static std::array<StructureType, 4> GetStructureTypes() { return { STRUCTURE_BASE, STRUCTURE_OUTPOST, STRUCTURE_STRONGHOLD, STRUCTURE_OBJECTIVE }; }
    static std::array<TileType, 12> GetTileTypes() { return { SPACE, FLOOR, WALL, DOOR_CLOSED, DOOR_OPEN, SOLDIER, MEDIC, INFECTED, BASE, OBJECTIVE, OUTPOST, STRONGHOLD }; }
    static std::array<UnitOrderType, 5> GetUnitOrderTypes() { return { MOVE, ROTATE_CLOCKWISE, ROTATE_COUNTER_CLOCKWISE, ATTACK, ATTACK_SPECIAL }; }
    static std::array<UnitType, 3> GetUnitTypes() { return { UNIT_SOLDIER, UNIT_MEDIC, UNIT_INFECTED }; }
    static std::array<FourPatchSections, 9> GetMapSections() { return { A, B, A_HM, C, D, C_VM, A_VM, B_HM, A_M }; }
};

