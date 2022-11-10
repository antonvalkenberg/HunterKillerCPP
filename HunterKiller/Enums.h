// ReSharper disable CommentTypo
// ReSharper disable IdentifierTypo
#pragma once

enum Direction {
    NORTH = 270, EAST = 0, SOUTH = 90, WEST = 180
};

enum Rotation {
    CLOCKWISE, COUNTER_CLOCKWISE
};

enum GameMode {
    CAPTURE, KILLING, KING_OF_THE_HILL, EXPLORATION, DEATHMATCH
};

enum MapType {
    DEFAULT, OPEN, NARROW
};

enum StructureOrderType {
    SPAWN_INFECTED, SPAWN_MEDIC, SPAWN_SOLDIER
};

enum StructureType {
    STRUCTURE_BASE, STRUCTURE_OUTPOST, STRUCTURE_STRONGHOLD, STRUCTURE_OBJECTIVE
};

enum TileType : char {
    SPACE = '.',
    FLOOR = '_',
    WALL = '|',
    DOOR_CLOSED = 'D',
    DOOR_OPEN = 'O',
    SOLDIER = 'S',
    MEDIC = 'M',
    INFECTED = 'I',
    BASE = 'B',
    OBJECTIVE = 'X',
    OUTPOST = 'P',
    STRONGHOLD = 'H'
};

enum UnitOrderType {
    MOVE, ROTATE_CLOCKWISE, ROTATE_COUNTER_CLOCKWISE, ATTACK, ATTACK_SPECIAL
};

enum UnitType {
    UNIT_SOLDIER, UNIT_MEDIC, UNIT_INFECTED
};

/**
 * The different sections of the final map.
 *
 * The data given to the map-construction is split up into 4 quadrants: A, B, C, D.
 *
 * The sections of the final map are defined up as follows:
 *
 * A | B | A
 *
 * C | D | C
 *
 * A | B | A
 *
 * Or in indices:
 *
 * 0 | 1 | 2
 *
 * 3 | 4 | 5
 *
 * 6 | 7 | 8
 *
 * Which corresponds with the value of this enum.
 *
 */
enum FourPatchSections
{
    /** Section 0, top-left which will be mirrored over to all corners of the final map. */
    A,
    /** Section 1, top-right, which will be mirrored over the horizontal axis into the final map. */
    B,
    /** section 2, the horizontal mirror of A */
    A_HM,
    /** Section 3, bottom-left, which will be mirrored over the vertical axis into the final map. */
    C,
    /** Section 4, bottom-right, which will become the center of the final map. */
    D,
    /** Section 5, the vertical mirror of C. */
    C_VM,
    /** Section 6, the vertical mirror of A. */
    A_VM,
    /** Section 7, the horizontal mirror of B. */
    B_HM,
    /** Section 8, the full mirror of A. */
    A_M
};