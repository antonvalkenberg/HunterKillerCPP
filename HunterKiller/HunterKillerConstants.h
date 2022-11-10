// ReSharper disable IdentifierTypo
// ReSharper disable CommentTypo
// ReSharper disable StringLiteralTypo
#pragma once
#include <random>
#include <string>

#include "Enums.h"
#include "MapLocation.h"

class HunterKillerConstants
{
public:
	inline static const std::string GAME_NAME = "HunterKiller";
	inline static constexpr int GAME_MIN_PLAYERS = 2;
	inline static constexpr int GAME_MAX_PLAYERS = 4;
	
	static inline std::random_device RD = std::random_device{};
	static inline std::default_random_engine RNG = std::default_random_engine{ RD() };

	// region Maps

	inline static const std::string DEFAULT_MAP_NAME = "default";
	inline static const std::string DEFAULT_MAP = "..........\n..........\n..||||||||\n..|B______\n..|_______\n..|_______\n..|_______\n..|_______\n..|_______\n..|_______";

	inline static const std::string TEST_MAP_PERFORMANCE = "B___\n____\n____\n____";
	inline static const std::string TEST_MAP_PERFORMANCE_NARROW = "B___\n_|D|\n_D__\n_|_|";
	inline static const std::string TEST_MAP_STRUCTURES = "6 5 S\nBM____\n_S____\n___P__\n______\n_|D||_\n_|_X|H";
	inline static const std::string TEST_MAP_WALLS = "B___|___\n____|___\n_|||||||\n_||_D___\n_|__|___\n_|D|||||\n_|__|___\n_|||||||\n____|___\n____|___";

	inline static const std::string MAP_CAPTURE_NAME = "capture";
	inline static const std::string MAP_CAPTURE = "8 5 S\n....................\n....................\n..|||||..||||||||||.\n..|B__|..|X______X|.\n..|___||||________||\n..|_________________\n..|_________________\n..|_________________\n..|_________________\n..|_________________\n..|_________________";
	inline static const std::string MAP_CAPTURE_NARROW_NAME = "capture narrow";
	inline static const std::string MAP_CAPTURE_NARROW = "15 9 S\n....................\n....................\n..||||...||||..||||.\n..|B_|...|_X|..|X_|.\n..|__|||||D||||||D||\n..||D|______________\n...|___|D|||D|_||D||\n...|_|||__|X_|_|X__|\n...|___|||||||_|||||\n...|||______________";
	inline static const std::string MAP_CAPTURE_OPEN_NAME = "capture open";
	inline static const std::string MAP_CAPTURE_OPEN = "8 5 S\n....................\n....................\n..||||||||||||||||||\n..|B________________\n..|_________________\n..|_______X______X__\n..|_________________\n..|_________________\n..|_________________\n..|_______X______X__";
	inline static const std::string MAP_KILLING_FLOOR_NAME = "killing floor";
	inline static const std::string MAP_KILLING_FLOOR = "..........\n..........\n..||||||..\n..|B___|..\n..|____|..\n..|____|..\n..|____|||\n..||______\n...|______\n...|______";
	inline static const std::string MAP_KILLING_FLOOR_NARROW_NAME = "killing floor narrow";
	inline static const std::string MAP_KILLING_FLOOR_NARROW = "..........\n..........\n..||||||..\n..|B___|..\n..|____|..\n..|____|..\n..|____|||\n..||______\n...|_||D||\n...|_|___|";
	inline static const std::string MAP_KILLING_FLOOR_OPEN_NAME = "killing floor open";
	inline static const std::string MAP_KILLING_FLOOR_OPEN = "..........\n..........\n..||||||||\n..|B______\n..|_______\n..|_______\n..|_______\n..|_______\n..|_______\n..|_______";
	inline static const std::string MAP_KING_NAME = "king of the hill";
	inline static const std::string MAP_KING = "8 8 S\n.........\n.||||||||\n.|B___|__\n.|____|__\n.||||D|__\n.|_______\n.|_______\n.|_______\n.|______H";
	inline static const std::string MAP_KING_NARROW_NAME = "king of the narrow hill";
	inline static const std::string MAP_KING_NARROW = "8 8 S\n.........\n.||||||||\n.|B___|__\n.|____|__\n.||||D|__\n.|_______\n.|_||D|||\n.|_|_____\n.|_|_||_H";
	inline static const std::string MAP_KING_OPEN_NAME = "king of the open hill";
	inline static const std::string MAP_KING_OPEN = "8 8 S\n.........\n.||||||||\n.|B______\n.|_______\n.|_______\n.|_______\n.|_______\n.|_______\n.|______H";
	inline static const std::string MAP_SPACESTATION_NAME = "spacestation";
	inline static const std::string MAP_SPACESTATION = "15 9 S\n........|||||...\n........|P__|...\n.|||||..|___|...\n.|B__|..|___|...\n.|___|||||D|||||\n.|_____________|\n.|______________\n.|______________\n.||__________|_|\n..|__________|X|";
	inline static const std::string MAP_SPACESTATION_NARROW_NAME = "spacestation narrow";
	inline static const std::string MAP_SPACESTATION_NARROW = "15 9 S\n........|||||...\n........|P__|...\n.|||||..|___|...\n.|B__|..|___|...\n.|___|||||D|||||\n.|___________D_|\n.|__||D||_||||__\n.|__|___|_D__|__\n.||_|___|_|_P|_|\n..|_D___D_||||X|";
	inline static const std::string MAP_SPACESTATION_OPEN_NAME = "spacestation open";
	inline static const std::string MAP_SPACESTATION_OPEN = "15 9 S\n........|||||...\n........|P__|...\n.||||||||___||||\n.|B_____________\n.|______________\n.|______________\n.|______________\n.|______________\n.||_____________\n..|_____________";
	inline static const std::string MAP_SQUAD_NAME = "squad battle";
	inline static const std::string MAP_SQUAD = "8 8 S\n50 0\n..........\n.|||||||||\n.|B___|___\n.|____|___\n.||||D|___\n.|________\n.|________\n.|________\n.|____||||\n.|____||||";
	inline static const std::string MAP_SQUAD_NARROW_NAME = "squad battle narrow";
	inline static const std::string MAP_SQUAD_NARROW = "8 8 S\n50 0\n..........\n.|||||||||\n.|B___|___\n.|____|___\n.|||D|||__\n.|________\n.|_|||||__\n.|_|___|__\n.|_D___|__\n.|_|___D__";
	inline static const std::string MAP_SQUAD_OPEN_NAME = "squad battle open";
	inline static const std::string MAP_SQUAD_OPEN = "8 8 S\n50 0\n..........\n.|||||||||\n.|B_______\n.|________\n.|________\n.|________\n.|________\n.|________\n.|________\n.|________";


	/*
	 * Defines the separator used to delineate lines within a string used for creating a MapSetup.
	 */
	inline static const std::string MAP_SETUP_LINE_SEPARATOR = "\n";
	/*
	 * Defines an option for map creation where players are not randomly assigned a map-section.
	 */
	inline static const std::string MAP_OPTION_NO_RANDOM_SECTIONS = "nonRandomSections";

	// endregion

	// region MapFeature

	/**
	 * The default amount of health points for a MapFeature.
	 */
	inline static constexpr int MAPFEATURE_DEFAULT_HP = 5;
	/**
	 * Default destructibility of a MapFeature.
	 */
	inline static constexpr bool MAPFEATURE_DEFAULT_DESTRUCTIBLE = false;
	/**
	 * Whether or not a MapFeature blocks Line of Sight by default.
	 */
	inline static constexpr bool MAPFEATURE_DEFAULT_BLOCKING_LOS = false;
	/**
	 * Whether or not a MapFeature can be moved over by default.
	 */
	inline static constexpr bool MAPFEATURE_DEFAULT_WALKABLE = true;

	// region Structure

	/**
	 * Structures are not controlled by any player by default.
	 */
	inline static constexpr int STRUCTURE_NO_CONTROL = -1;
	/**
	 * Health points for a structure.
	 */
	inline static constexpr int STRUCTURE_MAX_HP = 50;
	/**
	 * Structures are destructible.
	 */
	inline static constexpr bool STRUCTURE_DESTRUCTIBLE = true;
	/**
	 * Structures block Line of Sight.
	 */
	inline static constexpr bool STRUCTURE_BLOCKING_LOS = true;
	/**
	 * Structures can be moved over.
	 */
	inline static constexpr bool STRUCTURE_WALKABLE = true;
	/**
	 * Structures are capturable.
	 */
	inline static constexpr bool STRUCTURE_CAPTURABLE = true;
	/**
	 * Structures can spawn units.
	 */
	inline static constexpr bool STRUCTURE_ALLOW_SPAWNING = true;
	/**
	 * Structures generate resources.
	 */
	inline static constexpr bool STRUCTURE_GENERATES_RESOURCE = true;
	/**
	 * The default amount of resources a structure generates.
	 */
	inline static constexpr int STRUCTURE_RESOURCE_GENERATION = 12;
	/**
	 * Structures do not generate score points.
	 */
	inline static constexpr bool STRUCTURE_GENERATES_SCORE = false;
	/**
	 * The default amount of score points a structure generates.
	 */
	inline static constexpr int STRUCTURE_SCORE_GENERATION = 0;
	/**
	 * Structures are not command centers by default.
	 */
	inline static constexpr bool STRUCTURE_IS_COMMAND_CENTER = false;

	// region Base

	/**
	 * Bases can't be moved over.
	 */
	inline static constexpr bool BASE_WALKABLE = false;
	/**
	 * Bases can't be captured.
	 */
	inline static constexpr bool BASE_CAPTURABLE = false;
	/**
	 * Bases are command centers.
	 */
	inline static constexpr bool BASE_IS_COMMAND_CENTER = true;
	/*
	 * The default direction that Bases spawn their Units in.
	 */
	inline static constexpr Direction BASE_DEFAULT_SPAWN_DIRECTION = SOUTH;
	/*
	 * The distance from a Base that Units are spawned at.
	 */
	inline static constexpr int BASE_SPAWN_DISTANCE = 1;
	/**
	 * Bases generate an amount of resources equal to a default Structure's resource generation, unless specified otherwise by custom map parameters.
	 */
	inline static int BASE_RESOURCE_GENERATION = STRUCTURE_RESOURCE_GENERATION;
	static void Set_BASE_RESOURCE_GENERATION(const int amount) { BASE_RESOURCE_GENERATION = amount; }

	// endregion

	// region Outpost

	// Nothing different from default

	// endregion

	// region Stronghold

	/**
	 * Strongholds are tough.
	 */
	inline static constexpr int STRONGHOLD_MAX_HP = 250;
	/**
	 * Strongholds can't spawn units.
	 */
	inline static constexpr bool STRONGHOLD_ALLOW_SPAWNING = false;
	/**
	 * Strongholds generate more resources than usual.
	 */
	inline static constexpr int STRONGHOLD_RESOURCE_GENERATION = 25;

	// endregion

	// region Objective

	/**
	 * Objectives can't be destroyed.
	 */
	inline static constexpr bool OBJECTIVE_DESTRUCTIBLE = false;
	/**
	 * Objectives can't spawn units.
	 */
	inline static constexpr bool OBJECTIVE_ALLOW_SPAWNING = false;
	/**
	 * Objectives don't generate resources.
	 */
	inline static constexpr bool OBJECTIVE_GENERATES_RESOURCE = false;
	/**
	 * Objectives generate score points.
	 */
	inline static constexpr bool OBJECTIVE_GENERATES_SCORE = true;
	/**
	 * The amount of score points an objective generates.
	 */
	inline static constexpr int OBJECTIVE_SCORE_GENERATION = 50;

	// endregion

	// endregion

	// region Door

	/**
	 * Doors are indestructible.
	 */
	inline static constexpr bool DOOR_DESTRUCTIBLE = false;
	/**
	 * Door are created closed and block Line of Sight in that state.
	 */
	inline static constexpr bool DOOR_BLOCKS_LOS = true;
	/**
	 * Doors can be moved over. (At which point they will open and remain open for a number of rounds)
	 */
	inline static constexpr bool DOOR_WALKABLE = true;
	/**
	 * Once opened a door will remain open for this amount of rounds.
	 */
	inline static constexpr int DOOR_OPEN_ROUNDS = 5;

	// endregion

	// region Floor

	/**
	 * Floors are indestructible.
	 */
	inline static constexpr bool FLOOR_DESTRUCTIBLE = false;
	/**
	 * Floors do not block Line of Sight.
	 */
	inline static constexpr bool FLOOR_BLOCKS_LOS = false;
	/**
	 * Floors can be moved over.
	 */
	inline static constexpr bool FLOOR_WALKABLE = true;

	// endregion

	// region Space

	/**
	 * Space is indestructible.
	 */
	inline static constexpr bool SPACE_DESTRUCTIBLE = false;
	/**
	 * Space does not block Line of Sight.
	 */
	inline static constexpr bool SPACE_BLOCKS_LOS = false;
	/**
	 * Space can not be moved over.
	 */
	inline static constexpr bool SPACE_WALKABLE = false;

	// endregion

	// region Wall

	/**
	 * Walls are indestructible.
	 */
	inline static constexpr bool WALL_DESTRUCTIBLE = false;
	/**
	 * Walls block Line of Sight.
	 */
	inline static constexpr bool WALL_BLOCKS_LOS = true;
	/**
	 * Walls can not be moved over.
	 */
	inline static constexpr bool WALL_WALKABLE = false;

	// endregion

	// endregion

	// region Unit

	/**
	 * Default health points for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_HP = 5;
	/**
	 * Default orientation for a Unit.
	 */
	inline static Direction UNIT_DEFAULT_ORIENTATION = NORTH;
	/**
	 * Default Field of View range for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_FOV_RANGE = 3;
	/**
	 * Default Field of View angle for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_FOV_ANGLE = 90;
	/**
	 * Default attack range for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_ATTACK_RANGE = 4;
	/**
	 * Default attack damage for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_ATTACK_DAMAGE = 3;
	/**
	 * Default cooldown for a Unit's special attack.
	 */
	inline static constexpr int UNIT_DEFAULT_SPECIAL_COOLDOWN = 0;
	/**
	 * Default spawn cost for a Unit.
	 */
	inline static constexpr int UNIT_DEFAULT_SPAWN_COST = 5;
	/**
	 * Default score that a Unit is worth.
	 */
	inline static constexpr int UNIT_DEFAULT_SCORE = 25;
	/**
	 * The movement range for a Unit.
	 */
	inline static constexpr int UNIT_MOVEMENT_RANGE = 1;

	// region Infected

	/**
	 * Health points for an Infected.
	 */
	inline static constexpr int INFECTED_MAX_HP = 10;
	/**
	 * Field of View range for an Infected.
	 */
	inline static constexpr int INFECTED_FOV_RANGE = 4;
	/**
	 * Field of View angle for an Infected.
	 */
	inline static constexpr int INFECTED_FOV_ANGLE = 360;
	/**
	 * Attack range for an Infected.
	 */
	inline static constexpr int INFECTED_ATTACK_RANGE = 1;
	/**
	 * Attack damage for an Infected.
	 */
	inline static constexpr int INFECTED_ATTACK_DAMAGE = 5;
	/**
	 * Cooldown of an Infected's special attack.
	 */
	inline static constexpr int INFECTED_COOLDOWN = 2;
	/**
	 * Spawn cost for an Infected.
	 */
	inline static constexpr int INFECTED_SPAWN_COST = 10;
	/**
	 * The amount of points an Infected is worth when defeated.
	 */
	inline static constexpr int INFECTED_SCORE = 25;

	// endregion

	// region Medic

	/**
	 * Health points for a Medic.
	 */
	inline static constexpr int MEDIC_MAX_HP = 2;
	/**
	 * Field of View range for a Medic.
	 */
	inline static constexpr int MEDIC_FOV_RANGE = 3;
	/**
	 * Field of View angle for a Medic.
	 */
	inline static constexpr int MEDIC_FOV_ANGLE = 90;
	/**
	 * Attack range for a Medic.
	 */
	inline static constexpr int MEDIC_ATTACK_RANGE = 4;
	/**
	 * Attack damage for a Medic.
	 */
	inline static constexpr int MEDIC_ATTACK_DAMAGE = 1;
	/**
	 * Cooldown of a Medic's special attack.
	 */
	inline static constexpr int MEDIC_COOLDOWN = 2;
	/**
	 * Spawn cost for a Medic.
	 */
	inline static constexpr int MEDIC_SPAWN_COST = 10;
	/**
	 * The amount of points a Medic is worth when defeated.
	 */
	inline static constexpr int MEDIC_SCORE = 50;
	/**
	 * The amount of health points a Medic's special attack heals.
	 */
	inline static constexpr int MEDIC_SPECIAL_HEAL = 2;

	// endregion

	// region Soldier

	/**
	 * Health points for a Soldier.
	 */
	inline static constexpr int SOLDIER_MAX_HP = 5;
	/**
	 * Field of View range for a Soldier.
	 */
	inline static constexpr int SOLDIER_FOV_RANGE = 3;
	/**
	 * Field of View angle for a Soldier.
	 */
	inline static constexpr int SOLDIER_FOV_ANGLE = 90;
	/**
	 * Attack range for a Soldier.
	 */
	inline static constexpr int SOLDIER_ATTACK_RANGE = 4;
	/**
	 * Attack damage for a Soldier.
	 */
	inline static constexpr int SOLDIER_ATTACK_DAMAGE = 3;
	/**
	 * Cooldown of a Soldier's special attack
	 */
	inline static constexpr int SOLDIER_COOLDOWN = 20;
	/**
	 * Spawn cost for a Soldier.
	 */
	inline static constexpr int SOLDIER_SPAWN_COST = 5;
	/**
	 * The amount of points a Soldier is worth when defeated.
	 */
	inline static constexpr int SOLDIER_SCORE = 25;
	/**
	 * The damage of the Soldier's special attack.
	 */
	inline static constexpr int SOLDIER_SPECIAL_DAMAGE = 3;

	// endregion

	// endregion

	// region GameObject

	/**
	 * The default location if the object has not been placed yet.
	 */
	inline static MapLocation* GAMEOBJECT_NOT_PLACED = new MapLocation(-1, -1);
	/**
	 * The default amount of health points an object has.
	 */
	inline static constexpr int GAMEOBJECT_DEFAULT_HP = 1;

	// endregion

	// region Rules

	/**
	 * The frequency (in rounds) with which structures generate resources or points.
	 */
	inline static constexpr int RULES_STRUCTURE_GENERATION_FREQUENCY = 5;
	/**
	 * The maximum number of rounds a game will have.
	 */
	inline static constexpr int RULES_MAX_GAME_ROUNDS = 200;

	// endregion

	// region Map

	/**
	 * Amount of layers in our internal Map representation. Currently we have 2: MapFeatures and Units.
	 */
	inline static constexpr int MAP_INTERNAL_LAYERS = 2;
	/**
	 * The index of the MapFeature layer in our internal representation.
	 */
	inline static constexpr int MAP_INTERNAL_FEATURE_INDEX = 0;
	/**
	 * The index of the Unit layer in our internal representation.
	 */
	inline static constexpr int MAP_INTERNAL_UNIT_INDEX = 1;
	/**
	 * Defines the separator used to create space between the different layers in the Map's toString method.
	 */
	inline static const std::string MAP_TOSTRING_LAYER_SEPARATOR = " | ";

	// endregion

	// region Player

	/**
	 * The amount of resources a Player starts the game with.
	 */
	inline static constexpr int PLAYER_STARTING_RESOURCE = 10;

	// endregion

	// region Line of Sight

	/*
	 * A small float value to use when comparing floats
	 */
	inline static constexpr float LINE_OF_SIGHT_FLOAT_COMPARISON_THRESHOLD = 0.01f;

	// endregion
};

