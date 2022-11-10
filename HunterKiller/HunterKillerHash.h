#pragma once
#include "GameObject.h"
#include "HelperStructs.h"
#include "MapLocation.h"

struct MapLocationHash {
    size_t operator()(const MapLocation& rLocation) const { return rLocation.GetHashCode(); }
};

struct CacheEntryHash {
    size_t operator()(const LineOfSight_CacheEntry& rCacheEntry) const { return rCacheEntry.GetHashCode(); }
};

struct GameObjectIDHash {
    size_t operator()(const GameObject* pGameObject) const { return pGameObject->GetID(); }
};