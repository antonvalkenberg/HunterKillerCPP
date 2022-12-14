// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
#include "Structure.h"

#include "EnumExtensions.h"

Structure::Structure(MapLocation& rLocation, const StructureType type)
	: MapFeature(rLocation, HunterKillerConstants::STRUCTURE_MAX_HP, HunterKillerConstants::STRUCTURE_MAX_HP, HunterKillerConstants::STRUCTURE_DESTRUCTIBLE,
		HunterKillerConstants::STRUCTURE_BLOCKING_LOS, HunterKillerConstants::STRUCTURE_WALKABLE), TypeOfStructure(type)
{
	switch (type)
	{
	case STRUCTURE_BASE:
		IsCapturable = HunterKillerConstants::BASE_CAPTURABLE;
		SetWalkable(HunterKillerConstants::BASE_WALKABLE);
		IsCommandCenter = HunterKillerConstants::BASE_IS_COMMAND_CENTER;
		ResourceGeneration = HunterKillerConstants::BASE_RESOURCE_GENERATION;
		break;
	case STRUCTURE_OBJECTIVE:
		SetDestructible(HunterKillerConstants::OBJECTIVE_DESTRUCTIBLE);
		AllowsSpawning = HunterKillerConstants::OBJECTIVE_ALLOW_SPAWNING;
		GeneratesResource = HunterKillerConstants::OBJECTIVE_GENERATES_RESOURCE;
		GeneratesScore = HunterKillerConstants::OBJECTIVE_GENERATES_SCORE;
		ScoreGeneration = HunterKillerConstants::OBJECTIVE_SCORE_GENERATION;
		break;
	case STRUCTURE_OUTPOST:
		break;
	case STRUCTURE_STRONGHOLD:
		SetMaxHP(HunterKillerConstants::STRONGHOLD_MAX_HP);
		SetCurrentHP(HunterKillerConstants::STRONGHOLD_MAX_HP);
		AllowsSpawning = HunterKillerConstants::STRONGHOLD_ALLOW_SPAWNING;
		ResourceGeneration = HunterKillerConstants::STRONGHOLD_RESOURCE_GENERATION;
		break;
	default:
		throw std::invalid_argument("Unsupported Structure type on creation.");
	}
}

Structure::Structure(const Structure& rStructure)
	: MapFeature(rStructure.GetLocation(), rStructure.GetMaxHP(), rStructure.GetIsDestructible(), rStructure.IsBlockingLOS, rStructure.GetIsWalkable()), TypeOfStructure(rStructure.TypeOfStructure) {
	SetID(rStructure.GetID());
	ControllingPlayerID = rStructure.ControllingPlayerID;
	IsCapturable = rStructure.IsCapturable;
	AllowsSpawning = rStructure.AllowsSpawning;
	if (AllowsSpawning)
		SpawnLocation = rStructure.SpawnLocation;
	GeneratesResource = rStructure.GeneratesResource;
	ResourceGeneration = rStructure.ResourceGeneration;
	GeneratesScore = rStructure.GeneratesScore;
	ScoreGeneration = rStructure.ScoreGeneration;
	IsCommandCenter = rStructure.IsCommandCenter;
}

Structure::~Structure() {
    // We don't have ownership of the SpawnLocation's memory
    SpawnLocation = nullptr;
}

Structure* Structure::Copy() {
	return new Structure(*this);
}

std::string Structure::ToString() {
	switch (TypeOfStructure)
	{
	case STRUCTURE_BASE:
		return std::format("{}", static_cast<char>(BASE));
	case STRUCTURE_OBJECTIVE:
		return std::format("{}", static_cast<char>(OBJECTIVE));
	case STRUCTURE_OUTPOST:
		return std::format("{}", static_cast<char>(OUTPOST));
	case STRUCTURE_STRONGHOLD:
		return std::format("{}", static_cast<char>(STRONGHOLD));
	default:
		return "?";
	}
}
