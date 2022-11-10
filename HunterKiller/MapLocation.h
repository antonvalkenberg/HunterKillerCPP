// ReSharper disable IdentifierTypo
#pragma once
#include <cmath>
#include <format>
#include <optional>
#include <string>

#include "Enums.h"

class MapLocation
{
public:
    MapLocation() : X(0), Y(0) {}
    MapLocation(const int vX, const int vY) : X(vX), Y(vY) {}
    ~MapLocation() = default;
    bool operator==(const MapLocation& rLocation) const;
    bool operator!=(const MapLocation& rLocation) const;
    bool Equals(const MapLocation& rLocation) const;
    int GetHashCode() const { return 41 * X + 53 * Y; }
    std::string ToString() const { return std::format("[{0:d},{1:d}]", X, Y); }
    int GetX() const { return X; }
    int GetY() const { return Y; }
    static std::optional<Direction> GetDirectionTo(const MapLocation& rOrigin, const MapLocation& rTarget);
    static double GetEuclideanDistance(const int& rX1, const int& rY1, const int& rX2, const int& rY2) { return std::sqrt(std::pow(rX1 - rX2, 2) + std::pow(rY1 - rY2, 2)); }
    static double GetEuclideanDistance(const MapLocation& rLocation1, const MapLocation& rLocation2) { return GetEuclideanDistance(rLocation1.X, rLocation1.Y, rLocation2.X, rLocation2.Y); }
    static int GetManhattanDistance(const int& rX1, const int& rY1, const int& rX2, const int& rY2) { return std::abs(rX1 - rX2) + std::abs(rY1 - rY2); }
    static int GetManhattanDistance(const MapLocation& rLocation1, const MapLocation& rLocation2) { return GetManhattanDistance(rLocation1.X, rLocation1.Y, rLocation2.X, rLocation2.Y); }
private:
    int X;
    int Y;
};

