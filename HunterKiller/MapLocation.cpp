#include "MapLocation.h"

bool MapLocation::operator==(const MapLocation& rLocation) const
{
    return this->Equals(rLocation);
}

bool MapLocation::operator!=(const MapLocation& rLocation) const
{
    return !this->Equals(rLocation);
}

bool MapLocation::Equals(const MapLocation& rLocation) const
{
    return this->GetHashCode() == rLocation.GetHashCode();
}

std::optional<Direction> MapLocation::GetDirectionTo(const MapLocation& rOrigin, const MapLocation& rTarget)
{
    if (rOrigin.Equals(rTarget))
        return NORTH;

    const int dX = rOrigin.GetX() - rTarget.GetX();
    const int dY = rOrigin.GetY() - rTarget.GetY();

	// Check if the Y-coordinate of the origin is smaller than that of the target, and if the Xs were equal
	if (dY < 0 && dX == 0)
		// This means the direction from origin to target is increasing in Y, equal in X
		return SOUTH;
	
	// Check if the Yx were equal, and if the X-coordinate of the origin was greater than that of the target
	if (dY == 0 && dX > 0)
		// This means the direction from origin to target is decreasing in X, equal in Y
		return WEST;
	
	// Check if the Y-coordinate of the origin is greater than that of the target, and if the Xs were equal
	if (dY > 0 && dX == 0)
		// This means the direction from origin to target is decreasing in Y, equal in X
		return NORTH;
	
	if (dY == 0 && dX < 0)
		// That only leaves this one
		return EAST;

	return {};
}
