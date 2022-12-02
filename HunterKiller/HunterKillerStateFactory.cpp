// ReSharper disable StringLiteralTypo
// ReSharper disable CommentTypo
#include "HunterKillerStateFactory.h"

#include "EnumExtensions.h"
#include "Floor.h"
#include "Infected.h"
#include "Medic.h"
#include "Soldier.h"
#include "Space.h"
#include "Wall.h"

HunterKillerStateFactory::HunterKillerStateFactory()
{
    auto* pMapStrings = new std::vector{
        new std::pair(&HunterKillerConstants::MAP_CAPTURE_NAME, &HunterKillerConstants::MAP_CAPTURE),
        new std::pair(&HunterKillerConstants::MAP_CAPTURE_NARROW_NAME, &HunterKillerConstants::MAP_CAPTURE_NARROW),
        new std::pair(&HunterKillerConstants::MAP_CAPTURE_OPEN_NAME, &HunterKillerConstants::MAP_CAPTURE_OPEN),
        new std::pair(&HunterKillerConstants::MAP_KILLING_FLOOR_NAME, &HunterKillerConstants::MAP_KILLING_FLOOR),
        new std::pair(&HunterKillerConstants::MAP_KILLING_FLOOR_NARROW_NAME, &HunterKillerConstants::MAP_KILLING_FLOOR_NARROW),
        new std::pair(&HunterKillerConstants::MAP_KILLING_FLOOR_OPEN_NAME, &HunterKillerConstants::MAP_KILLING_FLOOR_OPEN),
        new std::pair(&HunterKillerConstants::MAP_KING_NAME, &HunterKillerConstants::MAP_KING),
        new std::pair(&HunterKillerConstants::MAP_KING_NARROW_NAME, &HunterKillerConstants::MAP_KING_NARROW),
        new std::pair(&HunterKillerConstants::MAP_KING_OPEN_NAME, &HunterKillerConstants::MAP_KING_OPEN),
        new std::pair(&HunterKillerConstants::MAP_SPACESTATION_NAME, &HunterKillerConstants::MAP_SPACESTATION),
        new std::pair(&HunterKillerConstants::MAP_SPACESTATION_NARROW_NAME, &HunterKillerConstants::MAP_SPACESTATION_NARROW),
        new std::pair(&HunterKillerConstants::MAP_SPACESTATION_OPEN_NAME, &HunterKillerConstants::MAP_SPACESTATION_OPEN),
        new std::pair(&HunterKillerConstants::MAP_SQUAD_NAME, &HunterKillerConstants::MAP_SQUAD),
        new std::pair(&HunterKillerConstants::MAP_SQUAD_NARROW_NAME, &HunterKillerConstants::MAP_SQUAD_NARROW),
        new std::pair(&HunterKillerConstants::MAP_SQUAD_OPEN_NAME, &HunterKillerConstants::MAP_SQUAD_OPEN),
    };
    MapRotation = new std::vector<MapSetup*>();
    for (const std::pair<const std::string*, const std::string*>* pMapStringPair : *pMapStrings)
    {
        auto* pRawMapData = new std::string(); //TODO: find solution for this, will leak?
        const auto* pMapLines = split_string(*pMapStringPair->second, HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
        // Check if we have any lines of settings
        if (std::isdigit(pMapLines->at(0)[0]))
        {
            // First line is the size/orientation
            const auto* pOptionsLine1 = split_string(pMapLines->at(0), " ");
            const int qWidth = std::stoi(pOptionsLine1->at(0));
            const int qHeight = std::stoi(pOptionsLine1->at(1));
            const Direction spawnDirection = EnumExtensions::TryParseDirection(pOptionsLine1->at(2)[0]);

            // Check if there is a second line of settings
            if (std::isdigit(pMapLines->at(1)[0]))
            {
                // Second line is the amount of starting resources for players and the base resource-generation
                const auto* pOptionsLine2 = split_string(pMapLines->at(1), " ");
                const int startingResource = std::stoi(pOptionsLine2->at(0));
                const int baseResourceGeneration = std::stoi(pOptionsLine2->at(1));
                for (size_t i = 2; i < pMapLines->size(); ++i)
                {
                    pRawMapData->append(pMapLines->at(i));
                    if (i != pMapLines->size() - 1) pRawMapData->append(HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
                }
                MapRotation->push_back(new MapSetup(*pMapStringPair->first, *pRawMapData, qWidth, qHeight, spawnDirection, startingResource, baseResourceGeneration));
                continue;
            }

            for (size_t i = 1; i < pMapLines->size(); ++i)
            {
                pRawMapData->append(pMapLines->at(i));
                if (i != pMapLines->size() - 1) pRawMapData->append(HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
            }
            MapRotation->push_back(new MapSetup(*pMapStringPair->first, *pRawMapData, qWidth, qHeight, spawnDirection));
            continue;
        }

        if (pMapLines->at(0).starts_with("custom"))
        {
            // Strip away the first line
            for (size_t i = 1; i < pMapLines->size(); ++i)
            {
                pRawMapData->append(pMapLines->at(i));
                if (i != pMapLines->size() - 1) pRawMapData->append(HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
            }
            MapRotation->push_back(new MapSetup(*pMapStringPair->first, *pRawMapData, true));
            continue;
        }

        MapRotation->push_back(new MapSetup(*pMapStringPair->first, *pMapStringPair->second, false));
    }
}

HunterKillerStateFactory::~HunterKillerStateFactory()
{
    if (Map) { delete Map; Map = nullptr; }
    if (MapSettings) { delete MapSettings; MapSettings = nullptr; }
    if (MapSectionPlayerIDMap) { delete MapSectionPlayerIDMap; MapSectionPlayerIDMap = nullptr; }
    if (MapRotation)
    {
        for (const MapSetup* pSetup : *MapRotation)
        {
            delete pSetup;
            pSetup = nullptr;
        }
        delete MapRotation; MapRotation = nullptr;
    }
}

HunterKillerState* HunterKillerStateFactory::GenerateInitialState(const std::vector<std::string*>& rPlayerNames, const std::string* pOptions)
{
    // Choose a map-setup from the rotation
    std::uniform_int_distribution random_map_setup(0, static_cast<int>(MapRotation->size()) - 1);
    MapSetup* pMapSetup = MapRotation->at(random_map_setup(HunterKillerConstants::RNG));

    return GenerateInitialStateFromSetup(rPlayerNames, *pMapSetup, pOptions);
}

HunterKillerState* HunterKillerStateFactory::GenerateInitialStateFromSetup(const std::vector<std::string*>& rPlayerNames, MapSetup& rMapSetup, const std::string* pOptions)
{
    const int nrPlayers = static_cast<int>(rPlayerNames.size());
    if (nrPlayers < HunterKillerConstants::GAME_MIN_PLAYERS || nrPlayers > HunterKillerConstants::GAME_MAX_PLAYERS)
        return nullptr;

    std::vector<int>* pPlayerSections;
    switch (nrPlayers)
    {
    case 4:
        // All four 'corners' are used for players
        pPlayerSections = new std::vector{ 0, 2, 6, 8 };
        break;
    case 3:
    {
        // In the case of 3 players, use a random one of the 2 semi-mirrored corners (index 2 and 6)
        std::uniform_int_distribution random_bool(0, 1);
        pPlayerSections = new std::vector{ 0, random_bool(HunterKillerConstants::RNG) ? 2 : 6, 8 };
        break;
    }
    case 2:
    default:
        // Only the two opposite corners
        pPlayerSections = new std::vector{ 0, 8 };
        break;
    }

    // Check if we need to randomise the sections, so on re-creation the same player does not end up in the same section each time.
    if (!pOptions || (pOptions && pOptions->find(HunterKillerConstants::MAP_OPTION_NO_RANDOM_SECTIONS) == std::string::npos))
        std::ranges::shuffle(*pPlayerSections, HunterKillerConstants::RNG);

    auto* pPlayers = new std::vector<HunterKillerPlayer*>();
    for (int i = 0; i < static_cast<int>(pPlayerSections->size()); ++i)
    {
        pPlayers->push_back(new HunterKillerPlayer(i, *rPlayerNames.at(i), pPlayerSections->at(i), rMapSetup.StartingResources));
    }
    delete pPlayerSections;
    pPlayerSections = nullptr;

    HunterKillerConstants::Set_BASE_RESOURCE_GENERATION(rMapSetup.BaseResourceGeneration);

    ConstructInternalMap(rMapSetup, *pPlayers);

    // Make sure the map assigns the objects to the players
    for (HunterKillerPlayer* pPlayer : *pPlayers)
    {
        for (GameObject* pObject : *Map->GetObjects() | std::views::values)
        {
            if (pObject)
            {
                // Check if it's a base and should be controlled by this player
                if (const Structure* pStructure = dynamic_cast<Structure*>(pObject); pStructure && pStructure->GetType() == STRUCTURE_BASE && pStructure->GetControllingPlayerID() == pPlayer->GetID())
                {
                    pPlayer->AssignCommandCenter(*pStructure);
                    pPlayer->AddStructure(pStructure->GetID());
                    // Other types are capturable and not controlled by any player at the start of the game
                }
                // Check if it's a unit and belongs to this player
                if (const Unit* pUnit = dynamic_cast<Unit*>(pObject); pUnit && pUnit->GetControllingPlayerID() == pPlayer->GetID())
                    pPlayer->AddUnit(pUnit->GetID());
            }
        }
    }

    Map->UpdateFieldOfView();

    auto* pState = new HunterKillerState(Map, pPlayers, 1, 0);

    Reset();

    return pState;
}

void HunterKillerStateFactory::CreateGameObjectInMap(const char data, const int x, const int y, const FourPatchSections section, const Direction baseSpawnDirection) const
{
    if (!Map || !MapSectionPlayerIDMap) {
        return;
    }

    MapLocation& rLocation = Map->ToLocation(x, y);

    // Some documentation about what happens in the following switch-case:
    // The MapFeature objects are mostly straightforward (except Structure, see below).
    // The Unit objects + Structure object are slightly more complicated, because the section index affects them:
    // - If the section index appears in our section-playerID map, actual Units/Bases need to be created.
    // - Otherwise they should be ignored.
    const bool ignoreUnitAndBase = !MapSectionPlayerIDMap->contains(section);
    switch (const TileType tile = EnumExtensions::TryParse(data))
    {
    case DOOR_CLOSED:
    {
        auto* pDoor = new Door(rLocation);
        Map->RegisterGameObject(pDoor);
        Map->Place(rLocation, pDoor);
        break;
    }
    case DOOR_OPEN:
    {
        auto* pOpenDoor = new Door(rLocation, HunterKillerConstants::DOOR_OPEN_ROUNDS);
        Map->RegisterGameObject(pOpenDoor);
        Map->Place(rLocation, pOpenDoor);
        break;
    }
    case FLOOR:
    {
        auto* pFloor = new Floor(rLocation);
        Map->RegisterGameObject(pFloor);
        Map->Place(rLocation, pFloor);
        break;
    }
    case SPACE:
    {
        auto* pSpace = new Space(rLocation);
        Map->RegisterGameObject(pSpace);
        Map->Place(rLocation, pSpace);
        break;
    }
    case WALL:
    {
        auto* pWall = new Wall(rLocation);
        Map->RegisterGameObject(pWall);
        Map->Place(rLocation, pWall);
        break;
    }
    case OBJECTIVE:
    {
        auto* pObjective = new Structure(rLocation, STRUCTURE_OBJECTIVE);
        Map->RegisterGameObject(pObjective);
        Map->Place(rLocation, pObjective);
        break;
    }
    case STRONGHOLD:
    {
        auto* pStronghold = new Structure(rLocation, STRUCTURE_STRONGHOLD);
        Map->RegisterGameObject(pStronghold);
        Map->Place(rLocation, pStronghold);
        break;
    }
    case OUTPOST:
    {
        auto* pOutpost = new Structure(rLocation, STRUCTURE_OUTPOST);
        pOutpost->SetSpawnLocation(rLocation);
        Map->RegisterGameObject(pOutpost);
        Map->Place(rLocation, pOutpost);
        break;
    }
    case INFECTED:
    case MEDIC:
    case SOLDIER:
    case BASE:
    {
        // Units always get a floor placed under them
        auto* pTempFloor = new Floor(rLocation);
        Map->RegisterGameObject(pTempFloor);
        Map->Place(rLocation, pTempFloor);

        // In case we need to ignore this unit/base, at least leave a Floor in place of it
        if (ignoreUnitAndBase)
            break;

        // Determine to which player-ID the Unit/Base will be assigned
        const int playerID = MapSectionPlayerIDMap->at(section);

        if (tile == INFECTED)
        {
            auto* pInfected = new Infected(playerID, rLocation, HunterKillerConstants::UNIT_DEFAULT_ORIENTATION);
            Map->RegisterGameObject(pInfected);
            Map->Place(rLocation, pInfected);
            break;
        }
        if (tile == MEDIC)
        {
            auto* pMedic = new Medic(playerID, rLocation, HunterKillerConstants::UNIT_DEFAULT_ORIENTATION);
            Map->RegisterGameObject(pMedic);
            Map->Place(rLocation, pMedic);
            break;
        }
        if (tile == SOLDIER)
        {
            auto* pSoldier = new Soldier(playerID, rLocation, HunterKillerConstants::UNIT_DEFAULT_ORIENTATION);
            Map->RegisterGameObject(pSoldier);
            Map->Place(rLocation, pSoldier);
            break;
        }

        // Base can't have a floor under it
        Map->UnregisterGameObject(pTempFloor);
        delete pTempFloor;
        pTempFloor = nullptr;

        // For Bases, we also need to determine the location of where they spawn Units.
        // This location is always adjacent to the base, in a predefined direction.
        // Initialise the spawn location with the location for the base defined in the FourPatch (section index 0).
        MapLocation* pSpawnLocation = Map->GetLocationInDirection(rLocation, baseSpawnDirection, HunterKillerConstants::BASE_SPAWN_DISTANCE);
        //if (!pSpawnLocation) break;
        switch (section)  // NOLINT(clang-diagnostic-switch-enum)
        {
            // We already know it's one of our player-IDs, so it can be only one of the following 4:
        case A:
            // Already initialised it for our patch-section, break out
            break;
        case A_HM:
            // Section 2 (top right of map), spawns in opposite direction when EAST or WEST
            if (baseSpawnDirection == NORTH || baseSpawnDirection == SOUTH)
                // spawning in same direction, so break out
                break;
        case A_VM:  // NOLINT(clang-diagnostic-implicit-fallthrough)
            // Section 6 (bottom left of map), spawns in opposite direction when NORTH or SOUTH
            if (baseSpawnDirection == WEST || baseSpawnDirection == EAST)
                // spawning in same direction, so break out
                break;
        case A_M:  // NOLINT(clang-diagnostic-implicit-fallthrough)
            // Section 8 always spawns in the opposite direction
        default:
            pSpawnLocation = Map->GetLocationInDirection(rLocation, EnumExtensions::TryGetOppositeDirection(baseSpawnDirection), HunterKillerConstants::BASE_SPAWN_DISTANCE);
        }

        // Now that we have defined our spawn location, we can create the base
        auto* pBase = new Structure(rLocation, STRUCTURE_BASE);
        pBase->SetControllingPlayerID(playerID);
        pBase->SetSpawnLocation(*pSpawnLocation);
        Map->RegisterGameObject(pBase);
        Map->Place(rLocation, pBase);
    }
    }
}

void HunterKillerStateFactory::SetupMap(const int width, const int height)
{
    if (!MapSettings)
        Map = new HunterKillerMap(HunterKillerConstants::GAME_NAME, width, height);
    else
        Map = new HunterKillerMap(MapSettings->Name, width, height);
}

std::map<int, int>* HunterKillerStateFactory::CreateMapSectionPlayerIDMapping(const std::vector<HunterKillerPlayer*>& rPlayers)
{
    auto* pMapSectionPlayerIDMap = new std::map<int, int>();
    for (const HunterKillerPlayer* pPlayer : rPlayers)
    {
        if (!pMapSectionPlayerIDMap->contains(pPlayer->GetMapSection()))
            pMapSectionPlayerIDMap->insert(std::pair(pPlayer->GetMapSection(), pPlayer->GetID()));
    }
    return pMapSectionPlayerIDMap;
}

void HunterKillerStateFactory::ConstructInternalMap(MapSetup& rMapSetup, const std::vector<HunterKillerPlayer*>& rPlayers)
{
    MapSettings = &rMapSetup;
    MapSectionPlayerIDMap = CreateMapSectionPlayerIDMapping(rPlayers);
    MirrorQuadrantsIntoMap();
}

/**
 * Represents the construction of a grid based on a predefined part.
 * This predefined part is used to build up the grid by mirroring and copying quadrants of it.
 *
 * Note: this function assumes the coordinate (0,0) to be the top-left point in the grid. The X-axis is increasing towards the right, while the Y-axis increases downward.
 *
 * The predefined part of the grid is assumed to have the following quadrants:
 * <pre>
 * A B
 * C D
 * </pre>
 * Where quadrants A has dimensions as defined by quadrantAWidth and quadrantAHeight. (either supplied by MapSetup, or determined in this function for custom maps)
 * - A is mirrored and copied over 3 times.
 * - B is mirrored and copied over only the X axis.
 * - C is mirrored and copied over only the Y axis.
 * - D is not copied at all.
 *
 * This results in the following grid:
 * <pre>
 * A B A
 * C D C
 * A B A
 * </pre>
 */
void HunterKillerStateFactory::MirrorQuadrantsIntoMap()
{
    if (!MapSettings)
        return;

    const std::vector<std::string>* pSplitMapData = split_string(MapSettings->MapData, HunterKillerConstants::MAP_SETUP_LINE_SEPARATOR);
    const int mapDataColumns = static_cast<int>(pSplitMapData->at(0).size());
    const int mapDataRows = static_cast<int>(pSplitMapData->size());

    // When we are constructing a custom map, we have to split up the provided map-data into four to act as Quadrant A.
    // Note: this assumes the provided map-data doesn't have rows of varying length...
    if (MapSettings->QuadrantAWidth < 0)
        MapSettings->QuadrantAWidth = MapSettings->Custom ? mapDataColumns / 2 : mapDataColumns;
    if (MapSettings->QuadrantAHeight < 0)
        MapSettings->QuadrantAHeight = MapSettings->Custom ? mapDataRows / 2 : mapDataRows;

    // Now we can determine the eventual size of our map
    const int mapWidth = MapSettings->Custom ? mapDataColumns : mapDataColumns + MapSettings->QuadrantAWidth;
    const int mapHeight = MapSettings->Custom ? mapDataRows : mapDataRows + MapSettings->QuadrantAHeight;

    // Now we know the dimensions of the Map we're creating
    SetupMap(mapWidth, mapHeight);

    // We need to determine the dimensions of Quadrants B, C and D.
    // Although the dimensions of Quadrant B define those of both C and D by definition.
    const int quadrantBWidth = mapWidth - MapSettings->QuadrantAWidth * 2;
    const int quadrantBHeight = mapHeight - MapSettings->QuadrantAHeight * 2;

    // Move through the predefined part of the grid. Note: using [y][x] again because of how we created the map-data collection
    // Note: To get a mirrored coordinate, the following steps are taken:
        // - take the total dimension of the grid (width for X, height for Y)
        // - minus 1 because coordinates start at 0
        // - minus the current value of the coordinate
    for (int y = 0; y < mapDataRows; y++) {
        const int columnsInThisRow = static_cast<int>(pSplitMapData->at(y).size());
        for (int x = 0; x < columnsInThisRow; x++) {
            const char gameObjectAsChar = pSplitMapData->at(y).at(x);
            if (MapSettings->Custom)
            {
                // Copy directly over, but classify in which section we are
                FourPatchSections section;
                if (x < MapSettings->QuadrantAWidth) {
                    // Either A, C, A_V
                    if (y < MapSettings->QuadrantAHeight)
                        section = A;
                    else if (y < MapSettings->QuadrantAHeight + quadrantBHeight)
                        section = C;
                    else
                        section = A_VM;
                }
                else if (x < MapSettings->QuadrantAWidth + quadrantBWidth) {
                    // Either B, D, B_Mirror
                    if (y < MapSettings->QuadrantAHeight)
                        section = B;
                    else if (y < MapSettings->QuadrantAHeight + quadrantBHeight)
                        section = D;
                    else
                        section = B_HM;
                }
                else {
                    // Either A_H, C_Mirror, or A_Mirror
                    if (y < MapSettings->QuadrantAHeight)
                        section = A_HM;
                    else if (y < MapSettings->QuadrantAHeight + quadrantBHeight)
                        section = C_VM;
                    else
                        section = A_M;
                }
                CreateGameObjectInMap(gameObjectAsChar, x, y, section, MapSettings->SpawnDirection);
            }
            else {
                // Check which quadrant this location on the predefined part is in.
                if (x < MapSettings->QuadrantAWidth && y < MapSettings->QuadrantAHeight) {
                    // Quadrant A, this exists 4 times on the grid.
                    // The first copy is on the same coordinates, in section 0.
                    CreateGameObjectInMap(gameObjectAsChar, x, y, A, MapSettings->SpawnDirection);
                    // The second copy is on the same y coordinate, but the x coordinate is mirrored, in section 2.
                    CreateGameObjectInMap(gameObjectAsChar, mapWidth - 1 - x, y, A_HM, MapSettings->SpawnDirection);
                    // The third copy is on the same x coordinate, but the y coordinate is mirrored, in section 6.
                    CreateGameObjectInMap(gameObjectAsChar, x, mapHeight - 1 - y, A_VM, MapSettings->SpawnDirection);
                    // The fourth copy is on mirrored x and mirrored y, in section 8.
                    CreateGameObjectInMap(gameObjectAsChar, mapWidth - 1 - x, mapHeight - 1 - y, A_M, MapSettings->SpawnDirection);
                }
                else if (x >= MapSettings->QuadrantAWidth && y < MapSettings->QuadrantAHeight) {
                    // Quadrant B, this exists twice on the grid, only mirrored in the Y axis.
                    // The first copy is on the same coordinates, in section 1.
                    CreateGameObjectInMap(gameObjectAsChar, x, y, B, MapSettings->SpawnDirection);
                    // The second copy is on the same x coordinate, but the y coordinate is mirrored, in section 7.
                    CreateGameObjectInMap(gameObjectAsChar, x, mapHeight - 1 - y, B_HM, MapSettings->SpawnDirection);
                }
                else if (x < MapSettings->QuadrantAWidth && y >= MapSettings->QuadrantAHeight) {
                    // Quadrant C, this exists twice on the grid, only mirrored in the X axis.
                    // The first copy is on the same coordinates, in section 3.
                    CreateGameObjectInMap(gameObjectAsChar, x, y, C, MapSettings->SpawnDirection);
                    // The second copy is on the same y coordinate, but the x coordinate is mirrored, in section 5.
                    CreateGameObjectInMap(gameObjectAsChar, mapWidth - 1 - x, y, C_VM, MapSettings->SpawnDirection);
                }
                else {
                    // Quadrant D, this exists only once on the grid, in section 4.
                    CreateGameObjectInMap(gameObjectAsChar, x, y, D, MapSettings->SpawnDirection);
                }
            }
        }
    }
    delete pSplitMapData; pSplitMapData = nullptr;
}

void HunterKillerStateFactory::Reset()
{
    // We don't own Map's memory, that is passed on to HunterKillerState
    Map = nullptr;
    // Map settings are created in the constructor and deleted in the destructor, don't want to do that here
    MapSettings = nullptr;
    delete MapSectionPlayerIDMap;
    MapSectionPlayerIDMap = nullptr;
}
