// ReSharper disable CommentTypo
#pragma once
#include <unordered_map>
#include <unordered_set>

#include "HelperStructs.h"
#include "HunterKillerHash.h"
#include "MapLocation.h"
#include "VectorHelpFunctions.h"

class HunterKillerMap;
typedef bool (HunterKillerMap::*BlocksLightFunction) (int, int) const;
typedef int (HunterKillerMap::*GetDistanceFunction) (int, int) const;

/**
 * Class representing the line of sight implementation for HunterKiller.
 * See: http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html
 * Several adjustments were made by CodePoKE.
 */
class LineOfSight
{
public:
    LineOfSight() : BlocksLight(nullptr), GetDistance(nullptr), FoVCache(nullptr) {}
    LineOfSight(bool (HunterKillerMap::*bl) (int, int) const, int (HunterKillerMap::*gd) (int, int) const) : BlocksLight(bl), GetDistance(gd) {
        FoVCache = new std::unordered_map<LineOfSight_CacheEntry, std::unordered_set<MapLocation, MapLocationHash>, CacheEntryHash>();
    }
    ~LineOfSight() { delete FoVCache; FoVCache = nullptr; }
    void Compute(MapLocation& rOrigin, int rangeLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap);
    void Compute(MapLocation& rOrigin, int rangeLimit, Direction facing, float angleLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap);
    void CacheVisibleLocations(const LineOfSight_CacheEntry& rEntry, const std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations) const;
    bool HaveCached(const LineOfSight_CacheEntry& rEntry) const;
    [[nodiscard]] std::unordered_set<MapLocation, MapLocationHash>* GetFromCache(const LineOfSight_CacheEntry& rEntry) const;
    [[nodiscard]] GetDistanceFunction GetDistanceType() const;
private:
    /*
    See: http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html
    @author Adam Milazzo
    @author Anton Valkenberg (anton.valkenberg@gmail.com)
    */
    void Compute(int octant, MapLocation& rMapOrigin, int rangeLimit, float facingAngle, float halfAngleLimit, int x, LineOfSight_Slope& top, LineOfSight_Slope& bottom, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap);
    bool IsAngleOutOfBounds(int x, int y, const MapLocation& rMapOrigin, float facingAngle, float halfAngleLimit) const;
    bool BlocksLightRef(int x, int y, int octant, const MapLocation& rMapOrigin, float facingAngle, float halfAngleLimit, HunterKillerMap& rMap);
    void SetVisibleRef(int x, int y, int octant, const MapLocation& rMapOrigin, float facingAngle, float halfAngleLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, const HunterKillerMap& rMap) const;
    static void TranslateOctantCoordsToMapCoords(int octantX, int octantY, int octant, int& rMapX, int& rMapY);
    static float MinimumAngleToOctant(int octant, Direction facing);
    static void NextOctant(int octant, std::pair<float, float>& vector);
    BlocksLightFunction BlocksLight;
    GetDistanceFunction GetDistance;
    static constexpr float NO_ANGLE_LIMIT = -1.0f;
    static constexpr int FULL_ANGLE_LIMIT = 180;
    static constexpr bool DEBUG = false;
    std::unordered_map<LineOfSight_CacheEntry, std::unordered_set<MapLocation, MapLocationHash>, CacheEntryHash>* FoVCache;
};