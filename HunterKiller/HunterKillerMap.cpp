// ReSharper disable CommentTypo
// ReSharper disable IdentifierTypo
// ReSharper disable StringLiteralTypo
// ReSharper disable CppClangTidyClangDiagnosticCoveredSwitchDefault
// ReSharper disable CppClangTidyBugproneImplicitWideningOfMultiplicationResult
#include "HunterKillerMap.h"

#include "Door.h"
#include "EnumExtensions.h"
#include "Wall.h"

HunterKillerMap::HunterKillerMap(const HunterKillerMap& rMap) : HunterKillerMap(rMap.GetName())
{
	MapWidth = rMap.GetMapWidth();
	MapHeight = rMap.GetMapHeight();

	Locations = new std::vector<MapLocation*>();
	for (int y = 0; y < MapHeight; ++y) {
		for (int x = 0; x < MapWidth; ++x) {
			Locations->push_back(new MapLocation(x, y));
		}
	}

	Objects = new std::unordered_map<int, GameObject*>();
	MapContent = new std::vector<std::vector<GameObject*>>();
	rMap.CopyMapContent(MapContent, Objects);
	
	IDBuffer = new std::vector(*rMap.GetIDBuffer());
	CommandCenterObjectIDs = new std::vector(*rMap.GetCommandCenterObjectIDs());
	
	LoS = new LineOfSight(&HunterKillerMap::BlocksLight, &HunterKillerMap::GetManhattanDistance);
}

HunterKillerMap::HunterKillerMap(const std::string& name, const int width, const int height) : Name(name), MapHeight(height), MapWidth(width)
{
	// Initializing the data collections like this because I can't get it to work with arrays...
	const int positions = MapHeight * MapWidth;
	MapContent = new std::vector<std::vector<GameObject*>>();
	MapContent->resize(positions);
	for (std::vector<GameObject*>& i : *MapContent) {
        i.resize(HunterKillerConstants::MAP_INTERNAL_LAYERS);
	}

	Objects = new std::unordered_map<int, GameObject*>();

	Locations = new std::vector<MapLocation*>();
	for (int y = 0; y < MapHeight; ++y) {
	    for (int x = 0; x < MapWidth; ++x) {
			Locations->push_back(new MapLocation(x, y));
	    }
	}
	
	IDBuffer = new std::vector<int>();
	CommandCenterObjectIDs = new std::vector<int>();
	
	LoS = new LineOfSight(&HunterKillerMap::BlocksLight, &HunterKillerMap::GetManhattanDistance);
}

HunterKillerMap::~HunterKillerMap()
{
	delete LoS;
	LoS = nullptr;
	delete CommandCenterObjectIDs;
	CommandCenterObjectIDs = nullptr;
	delete IDBuffer;
	IDBuffer = nullptr;

    for (const MapLocation* pLocation : *Locations) {
		delete pLocation;
		pLocation = nullptr;
	}

	for (auto pGameObject : *Objects | std::views::values) {
		delete pGameObject;
		pGameObject = nullptr;
	}
	delete Objects;
	Objects = nullptr;

	// All pointers to game objects in the MapContent collection have already been released by the Objects-loop above.
	delete MapContent;
	MapContent = nullptr;
}

int HunterKillerMap::GetHashCode() const
{
	// We've chosen a prime number close to 50 to make the hash code slightly more like a number that does not occur naturally
	int output = 43;

	for (const std::vector<GameObject*>& cell : *MapContent) {
		for (const GameObject* content : cell) {
			if (!content)
				continue;
			output ^= (content->GetID() ^ content->GetLocation().GetHashCode());
		}
	}

	return output;
}

bool HunterKillerMap::IsTraversable(const MapLocation& rLocation, std::string* pFailureReasons) const
{
    const int locationPosition = ToPosition(rLocation);

	// Check if the coordinates exist
	if (!IsXOnMap(rLocation.GetX())) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Location not traversable, X-coordinate is not on the map ({0:d}).\n", rLocation.GetX());
		return false;
	}
	if (!IsYOnMap(rLocation.GetY())) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Location not traversable, Y-coordinate is not on the map ({0:d}).\n", rLocation.GetY());
		return false;
	}

    const std::vector<GameObject*> locationList = MapContent->at(locationPosition);

	// There is a unit on a square if the content of the unit layer is not null
	if (locationList.at(HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX)) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Location {0:s} not traversable, Unit present.\n", rLocation.ToString());
		return false;
	}

	// A feature can be walked on/over if it is walkable, derp
	if (!dynamic_cast<MapFeature*>(locationList.at(HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX))->GetIsWalkable()) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Location {0:s} not traversable, MapFeature is not walkable.\n", rLocation.ToString());
		return false;
	}

	// If we got here, all is good
	return true;
}

bool HunterKillerMap::IsMovePossible(const MapLocation& rFromLocation, const UnitOrder& rMove, std::string* pFailureReasons) const
{
	// Check if the Unit layer at the location points to something
	if (!MapContent->at(ToPosition(rFromLocation)).at(HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX)) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Move not possible, no Unit on origin location {0:s}.\n", rFromLocation.ToString());
		return false;
	}

	// Make sure that the unit that is trying to move is actually at the location they are trying to move from
	if (rMove.GetObjectID() != MapContent->at(ToPosition(rFromLocation)).at(HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX)->GetID()) {
		if (pFailureReasons)
			*pFailureReasons += std::format("Move not possible, subject Unit (ID: {0:d}) is not on origin location {1:s}.\n", rMove.GetObjectID(), rFromLocation.ToString());
		return false;
	}

	if (!rMove.GetTargetLocation().has_value()) {
		if (pFailureReasons)
			*pFailureReasons += "Move not possible, no target location set.\n";
		return false;
	}

	// Check that the move's target location is a location next to the location that the Unit is in
    const MapLocation targetLocation = rMove.GetTargetLocation().value();
	for (const Direction direction : EnumExtensions::GetDirections()) {
        if (const MapLocation* adjacentLocation = GetAdjacentLocationInDirection(rFromLocation, direction); adjacentLocation && adjacentLocation->Equals(targetLocation))
			return IsTraversable(*adjacentLocation, pFailureReasons);
	}

	return false;
}

bool HunterKillerMap::IsMovePossible(const MapLocation& rFromLocation, const Direction direction) const
{
    const MapLocation* pTargetLocation = GetAdjacentLocationInDirection(rFromLocation, direction);
	if (!pTargetLocation)
		return false;
	return IsTraversable(*pTargetLocation);
}

bool HunterKillerMap::Move(const MapLocation& rTargetLocation, GameObject& rGameObject, std::string* pFailureReasons)
{
    const int targetPosition = ToPosition(rTargetLocation);
	
	if (!IsTraversable(rTargetLocation, pFailureReasons))
		return false;

	if (!dynamic_cast<Unit*>(&rGameObject)) {
		if (pFailureReasons)
			*pFailureReasons += "WARNING: Unable to move, object is not a Unit.\n";
	}

	bool success = Remove(rGameObject.GetLocation(), &rGameObject);
	if (success)
		success = Place(rTargetLocation, &rGameObject);

	if (const auto door = dynamic_cast<Door*>(MapContent->at(targetPosition)[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]); success && door && !door->IsOpen()) {
		door->Open();
		InvalidateFieldOfViewFor(door->GetLocation());
		UpdateFieldOfView();
	}

	return success;
}

MapLocation* HunterKillerMap::GetLocationInDirection(const MapLocation& rFromLocation, const Direction direction, const int distance) const
{
	if (distance > GetMaxTravelDistance(rFromLocation, direction))
		return nullptr;
    const int targetPosition = GetPositionInDirection(ToPosition(rFromLocation), direction, distance);
	return targetPosition >= 0 ? &ToLocation(targetPosition) : nullptr;
}

int HunterKillerMap::GetPositionInDirection(const int position, const Direction direction, const int distance) const
{
	int targetPosition = -1;
	if (distance < 0)
		return targetPosition;
	switch (direction)
	{
	case NORTH:
		// North is decreasing in Y, equal X. Or in positions: -(width * distance)
		targetPosition = position - (MapWidth * distance);
		break;
	case EAST:
		targetPosition = position + distance;
		break;
	case SOUTH:
		targetPosition = position + (MapWidth * distance);
		break;
	case WEST:
		targetPosition = position - distance;
		break;
	default:
		return targetPosition;
	}

	return targetPosition >= 0 && targetPosition < MapWidth * MapHeight ? targetPosition : -1;
}

int HunterKillerMap::GetMaxTravelDistance(const MapLocation& rLocation, const Direction direction) const
{
	switch (direction)
	{
	case NORTH:
		return rLocation.GetY();
	case EAST:
		// When traveling east, X is increasing and Y stays equal
		// This means we can only go as far as the width of the map, minus our current X-coordinate
		// (minus 1 because of indexes)
		return MapWidth - 1 - rLocation.GetX();
	case SOUTH:
		return MapHeight - 1 - rLocation.GetY();
	case WEST:
		return rLocation.GetX();
	default:
		return 0;
	}
}

void HunterKillerMap::GetNeighbours(const MapLocation& rLocation, std::unordered_set<MapLocation, MapLocationHash>& rNeighbourCollection) const
{
	for (const Direction direction : EnumExtensions::GetDirections()) {
        if (MapLocation* adjacent = GetAdjacentLocationInDirection(rLocation, direction))
			rNeighbourCollection.emplace(*adjacent);
	}
}

void HunterKillerMap::GetAreaAround(MapLocation& rLocation, const bool includeCentre, std::unordered_set<MapLocation, MapLocationHash>& rAreaCollection) const
{
	GetNeighbours(rLocation, rAreaCollection);

	// Get the corner positions (North-East, South-East, South-West, North-West)
	// North-East is increasing X, decreasing Y.
	if (IsXOnMap(rLocation.GetX() + 1) && IsYOnMap(rLocation.GetY() - 1))
		rAreaCollection.emplace(ToLocation(rLocation.GetX() + 1, rLocation.GetY() - 1));
	// South-East is increasing X, increasing Y.
	if (IsXOnMap(rLocation.GetX() + 1) && IsYOnMap(rLocation.GetY() + 1))
		rAreaCollection.emplace(ToLocation(rLocation.GetX() + 1, rLocation.GetY() + 1));
	// South-West is decreasing X, increasing Y.
	if (IsXOnMap(rLocation.GetX() - 1) && IsYOnMap(rLocation.GetY() + 1))
		rAreaCollection.emplace(ToLocation(rLocation.GetX() - 1, rLocation.GetY() + 1));
	// North-West is decreasing X, decreasing Y.
	if (IsXOnMap(rLocation.GetX() - 1) && IsYOnMap(rLocation.GetY() - 1))
		rAreaCollection.emplace(ToLocation(rLocation.GetX() - 1, rLocation.GetY() - 1));

	if (includeCentre)
		rAreaCollection.emplace(rLocation);
}

void HunterKillerMap::GetMapFeaturesAround(const MapLocation& rLocation, std::vector<std::vector<MapFeature*>>& rAreaCollection) const
{
	// Make sure the collection is appropriately sized
	if (rAreaCollection.size() != 3 || rAreaCollection[0].size() != 3 || rAreaCollection[1].size() != 3 || rAreaCollection[2].size() != 3)
		return;
	
	if (!IsOnMap(rLocation))
		return;

	// Index 0,0 is -1,-1
	if (IsXOnMap(rLocation.GetX() - 1) && IsYOnMap(rLocation.GetY() - 1)) {
		rAreaCollection[0][0] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() - 1, rLocation.GetY() - 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 0,1 is 0,-1
	if (IsXOnMap(rLocation.GetX()) && IsYOnMap(rLocation.GetY() - 1)) {
		rAreaCollection[0][1] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX(), rLocation.GetY() - 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 0,2 is +1,-1
	if (IsXOnMap(rLocation.GetX() + 1) && IsYOnMap(rLocation.GetY() - 1)) {
		rAreaCollection[0][2] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() + 1, rLocation.GetY() - 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 1,0 is -1,0
	if (IsXOnMap(rLocation.GetX() - 1) && IsYOnMap(rLocation.GetY())) {
		rAreaCollection[1][0] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() - 1, rLocation.GetY()))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 1,1 is 0,0
	rAreaCollection[1][1] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	// Index 1,2 is +1,0
	if (IsXOnMap(rLocation.GetX() + 1) && IsYOnMap(rLocation.GetY())) {
		rAreaCollection[1][2] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() + 1, rLocation.GetY()))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 2,0 is -1,+1
	if (IsXOnMap(rLocation.GetX() - 1) && IsYOnMap(rLocation.GetY() + 1)) {
		rAreaCollection[2][0] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() - 1, rLocation.GetY() + 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 2,1 is 0,+1
	if (IsXOnMap(rLocation.GetX()) && IsYOnMap(rLocation.GetY() + 1)) {
		rAreaCollection[2][1] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX(), rLocation.GetY() + 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
	// Index 2,2 is +1,+1
	if (IsXOnMap(rLocation.GetX() + 1) && IsYOnMap(rLocation.GetY() + 1)) {
		rAreaCollection[2][2] = dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation.GetX() + 1, rLocation.GetY() + 1))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	}
}

std::unordered_set<MapLocation, MapLocationHash>* HunterKillerMap::GetFieldOfView(const Unit& rUnit) {
	if (rUnit.IsFieldOfViewValid())
		return rUnit.GetFieldOfView();

    // Note: Field-of-View caching for improved simulation performance. Cannot be used on maps with Doors.
	//if (const auto pEntry = new LineOfSight_CacheEntry(rUnit.GetLocation(), rUnit.GetFieldOfViewRange(), rUnit.GetOrientation(), static_cast<float>(rUnit.GetFieldOfViewAngle())); LoS->HaveCached(*pEntry))
		//return LoS->GetFromCache(*pEntry);

    const auto pLocations = new std::unordered_set<MapLocation, MapLocationHash>();
	LoS->Compute(rUnit.GetLocation(), rUnit.GetFieldOfViewRange(), rUnit.GetOrientation(), static_cast<float>(rUnit.GetFieldOfViewAngle()), *pLocations, *this);
	
	return pLocations;
}

std::unordered_set<MapLocation, MapLocationHash>* HunterKillerMap::GetFieldOfView(const Structure& rStructure) const
{
    const auto pLocations = new std::unordered_set<MapLocation, MapLocationHash>();
	GetAreaAround(rStructure.GetLocation(), true, *pLocations);
	return pLocations;
}

MapLocation* HunterKillerMap::GetObjectLocation(const int objectID) const
{
	if (!Objects->contains(objectID))
		return nullptr;

	return &(Objects->at(objectID)->GetLocation());
}

Unit* HunterKillerMap::GetUnitAtLocation(const MapLocation& rLocation) const
{
	if (IsOnMap(rLocation) && MapContent->at(ToPosition(rLocation))[HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX])
		return dynamic_cast<Unit*>(MapContent->at(ToPosition(rLocation))[HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX]);
	
	return nullptr;
}

MapFeature* HunterKillerMap::GetFeatureAtLocation(const MapLocation& rLocation) const
{
	if (IsOnMap(rLocation))
		return dynamic_cast<MapFeature*>(MapContent->at(ToPosition(rLocation))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]);
	return nullptr;
}

bool HunterKillerMap::IsAttackOrderWithoutTarget(const UnitOrder& rOrder) const
{
	if (!rOrder.IsAttackOrder() || !rOrder.GetTargetLocation().has_value())
		return false;

	return !GetUnitAtLocation(rOrder.GetTargetLocation().value()) && !dynamic_cast<Structure*>(GetFeatureAtLocation(rOrder.GetTargetLocation().value()));
}

bool HunterKillerMap::IsAttackOrderTargetingAllyStructure(const UnitOrder& rOrder, const Unit* pUnit) const
{
	if (!rOrder.IsAttackOrder() || !rOrder.GetTargetLocation().has_value())
		return false;
	auto* pFeature = GetFeatureAtLocation(rOrder.GetTargetLocation().value());
	if (!pFeature) return false;
    const auto* pStructure = dynamic_cast<Structure*>(pFeature);
	return pStructure && pStructure->GetControllingPlayerID() == pUnit->GetControllingPlayerID();
}

bool HunterKillerMap::IsAttackOrderTargetingAllyUnit(const UnitOrder& rOrder, const Unit* pUnit) const
{
	if (!rOrder.IsAttackOrder() || !rOrder.GetTargetLocation().has_value())
		return false;
    const auto* pTargetUnit = GetUnitAtLocation(rOrder.GetTargetLocation().value());
	return pTargetUnit && pTargetUnit->GetControllingPlayerID() == pUnit->GetControllingPlayerID();
}

// Ownership of GameObject memory is passed to here
void HunterKillerMap::RegisterGameObject(GameObject* pGameObject) const
{
	int objectID;
	if (!IDBuffer->empty()) {
		objectID = IDBuffer->front();
		IDBuffer->erase(IDBuffer->begin());
	}
	else
		objectID = static_cast<int>(Objects->size());

	pGameObject->SetID(objectID);
	Objects->insert(std::pair(objectID, pGameObject));

	if (const Structure* pStructure = dynamic_cast<Structure*>(pGameObject); pStructure && pStructure->GetIsCommandCenter())
		CommandCenterObjectIDs->push_back(objectID);
}

void HunterKillerMap::UnregisterGameObject(GameObject* pGameObject) const {
    const int objectID = pGameObject->GetID();
	Objects->erase(objectID);
	IDBuffer->push_back(objectID);
	Remove(pGameObject->GetLocation(), pGameObject);

    if (const Structure* pStructure = dynamic_cast<Structure*>(pGameObject); pStructure && pStructure->GetIsCommandCenter())
		CommandCenterObjectIDs->erase(std::ranges::remove(*CommandCenterObjectIDs, objectID).begin(), CommandCenterObjectIDs->end());
}

void HunterKillerMap::UpdateFieldOfView() {
	for (const std::pair<int, GameObject*> kv : *Objects) {
        const auto pUnit = dynamic_cast<Unit*>(kv.second);
		if (!pUnit)
			continue;

		if (pUnit->IsFieldOfViewValid())
			continue;
		
		pUnit->UpdateFieldOfView(GetFieldOfView(*pUnit));
	}
}

void HunterKillerMap::InvalidateFieldOfViewFor(const MapLocation& rLocation) {
	for (const std::pair<int, GameObject*> kv : *Objects) {
        const auto pUnit = dynamic_cast<Unit*>(kv.second);
		if (pUnit && !pUnit->IsFieldOfViewValid())
			continue;
		if (pUnit) {
            if (const auto pLocations = GetFieldOfView(*pUnit); pLocations->contains(rLocation))
				pUnit->InvalidateFieldOfView();
		}
	}
}

bool HunterKillerMap::Place(const int position, GameObject* pObject) const
{
	int layer;
	if (dynamic_cast<MapFeature*>(pObject))
		layer = HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX;
	else if (dynamic_cast<Unit*>(pObject))
		layer = HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX;
	else
		return false;

	// Check if anything is placed here already
	if (MapContent->at(position)[layer])
		return false;

	pObject->SetLocation(ToLocation(position));
	MapContent->at(position)[layer] = pObject;
	return true;
}

bool HunterKillerMap::Remove(const int position, GameObject* pObject) const
{
	int layer;
	if (dynamic_cast<MapFeature*>(pObject))
		layer = HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX;
	else if (dynamic_cast<Unit*>(pObject))
		layer = HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX;
	else
		return false;

	if (!MapContent->at(position)[layer])
		return false;
	if (MapContent->at(position)[layer]->GetID() != pObject->GetID())
		return false;

	MapContent->at(position)[layer] = nullptr;
	return true;
}

std::vector<MapLocation*>* HunterKillerMap::FindPath(const MapLocation& rFrom, const MapLocation& rTo) const
{
	std::multimap<int, HunterKillerMap_PathNode*> open;
	std::unordered_map<int, HunterKillerMap_PathNode*> nodes;
	std::vector<int> path;

	int rootPosition = ToPosition(rFrom);
    const int targetPosition = ToPosition(rTo);

    auto* pRoot = new HunterKillerMap_PathNode(new IntPayload(0));
	pRoot->Position = rootPosition;
	nodes.emplace(rootPosition, pRoot);
	open.emplace(0, pRoot);

	while (!open.empty()) {
		HunterKillerMap_PathNode* pNode = open.begin()->second;
		open.erase(open.begin());
		// If we have reached the target, go back and add all parent nodes to the path.
		if (pNode->Position == targetPosition) {
			while (!pNode->IsRoot()) {
				path.push_back(pNode->Position);
				pNode = (HunterKillerMap_PathNode*)pNode->GetParent();
			}
			break;
		}
		pNode->Closed = true;

		MapLocation& rNodeLocation = ToLocation(pNode->Position);
		for (const Direction move : EnumExtensions::GetDirections()) {
			// Check if this move is possible.
			// Note: this causes searches that have a location that cannot be traversed to not yield a solution.
			if (!IsMovePossible(rNodeLocation, move))
				continue;

            if (const MapLocation* pMoveLocation = GetLocationInDirection(rNodeLocation, move, 1))
				AddNode(nodes, open, *pNode, *pMoveLocation, 1, rTo);
		}
	}

    const auto pPathLocations = new std::vector<MapLocation*>();
	for (auto i = path.rbegin(); i != path.rend(); ++i) {
		pPathLocations->push_back(&ToLocation(*i));
	}

	return pPathLocations;
}

std::string HunterKillerMap::ToString() const
{
	std::string mapString = std::format("{0:s}.[{1:d}]\n", Name, GetHashCode());
	for (int y = 0; y < MapHeight; y++)
	{
		std::string lineString;
		lineString += HunterKillerConstants::MAP_TOSTRING_LAYER_SEPARATOR;
		for (int x = 0; x < MapWidth; x++)
		{
            const int currentPosition = ToPosition(x, y);
			std::vector<GameObject*>& rObjects = MapContent->at(currentPosition);
			std::string featureLevel = rObjects[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX] ? rObjects[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX]->ToString() : " ";
            const std::string unitLevel = rObjects[HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX] ? rObjects[HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX]->ToString() : ".";
			lineString.insert(x, featureLevel);
			lineString += unitLevel;
		}
		mapString += (lineString + "\n");
	}
	return mapString;
}

void HunterKillerMap::Timer()
{
	for (const auto pObject : *Objects | std::views::values) {
		if (auto* pDoor = dynamic_cast<Door*>(pObject); pDoor && pDoor->IsOpen()) {
			pDoor->ReduceTimer();

			if (pDoor->GetOpenTimer() <= 0 && !pDoor->GetIsBlockingLOS() && TryCloseDoor(*pDoor))
				InvalidateFieldOfViewFor(pDoor->GetLocation());
		}
		else
			if (auto* pUnit = dynamic_cast<Unit*>(pObject); pUnit)
				pUnit->ReduceCooldown();
	}
}

bool HunterKillerMap::TryCloseDoor(Door& rDoor) const
{
	if (GetUnitAtLocation(rDoor.GetLocation()))
	{
		rDoor.KeepOpen();
		return false;
	}
	rDoor.Close();
	return true;
}

bool HunterKillerMap::BlocksLight(const int x, const int y) const
{
	if (!IsXOnMap(x) || !IsYOnMap(y))
		return true;
	return dynamic_cast<MapFeature*>(MapContent->at(ToPosition(x, y))[HunterKillerConstants::MAP_INTERNAL_FEATURE_INDEX])->GetIsBlockingLOS();
}

void HunterKillerMap::AddNode(std::unordered_map<int, HunterKillerMap_PathNode*>& rNodes, std::multimap<int, HunterKillerMap_PathNode*>& rOpen, HunterKillerMap_PathNode& rParent, const MapLocation& rLocation, const int cost, const MapLocation& rTarget) const
{
	int locationPosition = ToPosition(rLocation);

    if (const Wall* pWall = dynamic_cast<Wall*>(GetFeatureAtLocation(rLocation)); pWall && !pWall->GetIsDestructible()) {
		pWall = nullptr;
		return;
	}

    const int pathCost = rParent.PathCost->GetValue() + cost;
	int estimatedCostToTarget = pathCost + MapLocation::GetManhattanDistance(rTarget, rLocation);

	if (rNodes.contains(locationPosition)) {
        const HunterKillerMap_PathNode* pNode = rNodes.at(locationPosition);
        if (const int currentPathCost = pNode->PathCost->GetValue(); !pNode->Closed && pathCost < currentPathCost) {
			// We need to find the current entry of this location in the open-map
            const auto [rangeStart, rangeEnd] = rOpen.equal_range(pNode->EstimatedCostToTarget);
			for (std::multimap<int, HunterKillerMap_PathNode*>::iterator i = rangeStart; i != rangeEnd; ++i) {
				if (i->second->Position == locationPosition) {
					rOpen.erase(i);
					break;
				}
			}
			// Insert again with new costs
			auto* pNewNode = new HunterKillerMap_PathNode(new IntPayload(pathCost), &rParent);
			pNewNode->Position = locationPosition;
			pNewNode->EstimatedCostToTarget = estimatedCostToTarget;
			rOpen.emplace(estimatedCostToTarget, pNewNode);
		}
		return;
	}

	auto* pNode = new HunterKillerMap_PathNode(new IntPayload(pathCost), &rParent);
	pNode->Position = locationPosition;
	pNode->EstimatedCostToTarget = estimatedCostToTarget;
	
	rNodes.insert(std::pair(locationPosition, pNode));
	rOpen.insert(std::pair(estimatedCostToTarget, pNode));
}

void HunterKillerMap::CopyMapContent(std::vector<std::vector<GameObject*>>* pCopyTo, std::unordered_map<int, GameObject*>* pCopiedObjects) const
{
	const int positions = MapHeight * MapWidth;
	pCopyTo->clear();
	pCopyTo->resize(positions);
	pCopiedObjects->clear();

	for (int pos = 0; pos < positions; ++pos) {
		pCopyTo->at(pos).resize(HunterKillerConstants::MAP_INTERNAL_LAYERS);
	    for (int layer = 0; layer < HunterKillerConstants::MAP_INTERNAL_LAYERS; ++layer) {
            if (GameObject* pObject = MapContent->at(pos)[layer]) {
				GameObject* pCopy = pObject->Copy();
				pCopiedObjects->insert(std::pair(pCopy->GetID(), pCopy));
				pCopyTo->at(pos).at(layer) = pCopy;
			}
	    }
	}
}

bool HunterKillerMap::AttackLocation(const MapLocation& rLocation, const int damage) const
{
	if (!IsXOnMap(rLocation.GetX()) || !IsYOnMap(rLocation.GetY()))
		return false;

	if (MapFeature* pFeature = GetFeatureAtLocation(rLocation); pFeature && pFeature->GetIsDestructible())
		pFeature->ReduceHP(damage);

	if (Unit* pUnit = GetUnitAtLocation(rLocation); pUnit)
		pUnit->ReduceHP(damage);

	return true;
}

void HunterKillerMap::Prepare(int activePlayerID, std::unordered_set<MapLocation, MapLocationHash>& rPlayerFieldOfView, std::vector<int>& rRemovedUnitIDs)
{
	for (auto* pMapLocation : *Locations) {
		// Check if this location lies outside of the player's field-of-view
		if (rPlayerFieldOfView.contains(*pMapLocation))
			continue;
		
		// Check if the unit belongs to another player
		if (auto* pUnit = dynamic_cast<Unit*>(MapContent->at(ToPosition(*pMapLocation))[HunterKillerConstants::MAP_INTERNAL_UNIT_INDEX]); pUnit && pUnit->GetControllingPlayerID() != activePlayerID) {
			// Remove the unit from the map
			rRemovedUnitIDs.push_back(pUnit->GetID());
			UnregisterGameObject(pUnit);
			IDBuffer->erase(std::find(IDBuffer->begin(), IDBuffer->end(), pUnit->GetID()));
			delete pUnit; pUnit = nullptr;
		}
	}
}
