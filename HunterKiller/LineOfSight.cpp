// ReSharper disable CommentTypo
// ReSharper disable CppClangTidyBugproneBranchClone
#include "LineOfSight.h"

#include <iostream>

#include "HunterKillerMap.h"

void LineOfSight::Compute(MapLocation& rOrigin, const int rangeLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap)
{
    Compute(rOrigin, rangeLimit, NORTH, NO_ANGLE_LIMIT, rVisibleLocations, rMap);
}

void LineOfSight::Compute(MapLocation& rOrigin, const int rangeLimit, const Direction facing, const float angleLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap)
{
	rVisibleLocations.emplace(rOrigin);

    const float facingAngle = static_cast<float>(facing);
    const float halfAngleLimit = angleLimit / 2.0f;

    // Make a new vector set to the facing angle.
    std::pair targetVector(1.0f, 0.0f);
    VectorHelpFunctions::SetAngleDeg(targetVector, facingAngle);
	// And a reference vector to use when checking angles between different octants.
	std::pair referenceVector(1.0f, 0.0f);

	// Check if we can skip calculations for any octant that is out the angle-limit's bounds.
    for (int octant = 0; octant < 8; octant++)
    {
		// Skip this check for a limit that signifies full vision, or has no limit.
        if (halfAngleLimit < FULL_ANGLE_LIMIT || fabsf(angleLimit - NO_ANGLE_LIMIT) < HunterKillerConstants::LINE_OF_SIGHT_FLOAT_COMPARISON_THRESHOLD) {
			// Check if we have 'cached' the limits for this octant and Direction combination.
			float angle = MinimumAngleToOctant(octant, facing);

			if (angle == std::nanf("")) {
				NextOctant(octant, referenceVector);
				float upper = std::abs(VectorHelpFunctions::AngleDeg(targetVector, referenceVector));
				NextOctant((octant + 1) % 8, referenceVector);
				float lower = std::abs(VectorHelpFunctions::AngleDeg(targetVector, referenceVector));
				angle = std::ceilf(std::min(lower, upper));
			}

			// If the minimum angle to the octant is larger than half of the angle limit, we can skip calculations for this octant, since it lies out of bounds.
			if (angle > halfAngleLimit)
				continue;
        }

		LineOfSight_Slope top(1, 1);
		LineOfSight_Slope bottom(0, 1);
        Compute(octant, rOrigin, rangeLimit, facingAngle, halfAngleLimit, 1, top, bottom, rVisibleLocations, rMap);
    }
}

void LineOfSight::Compute(const int octant, MapLocation& rMapOrigin, const int rangeLimit, const float facingAngle, const float halfAngleLimit, int x, LineOfSight_Slope& top, LineOfSight_Slope& bottom, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, HunterKillerMap& rMap)
{
	// throughout this function there are references to various parts of tiles. a tile's coordinates refer to its centre,
	// and the following diagram shows the parts of the tile and the vectors from the origin that pass through those parts.
	// given a part of a tile with vector u, a vector v passes above it if v > u and below it if v < u
	//		g		  centre:			y / x
	//  a------b	a top left:			(y*2+1) / (x*2-1)		i inner top left: (y*4+1) / (x*4-1)
	//  |  /\  |	b top right:		(y*2+1) / (x*2+1)		j inner top right: (y*4+1) / (x*4+1)
	//  |i/__\j|	c bottom left:		(y*2-1) / (x*2-1)		k inner bottom left: (y*4-1) / (x*4-1)
	// e|/|  |\|f	d bottom right:		(y*2-1) / (x*2+1)		m inner bottom right: (y*4-1) / (x*4+1)
	//  |\|__|/|	e middle left:		(y*2) / (x*2-1)
	//  |k\  /m|	f middle right:		(y*2) / (x*2+1)			a-d are the corners of the tile
	//  |  \/  |	g top centre:		(y*2+1) / (x*2)			e-h are the corners of the inner (wall) diamond
	//  c------d	h bottom centre:	(y*2-1) / (x*2)			i-m are the corners of the inner square (1/2 tile width)
	//		h
	for (; x <= rangeLimit; x++) // (x <= (int)rangeLimit) == (rangeLimit < 0 || x <= rangeLimit)
	{
		// compute the Y coordinates of the top and bottom of the sector. we maintain that top > bottom
		int topY;
		if (top.X == 1) // if top == ?/1 then it must be 1/1 because 0/1 < top <= 1/1. this is special-cased because top
		{               // starts at 1/1 and remains 1/1 as long as it doesn't hit anything, so it's a common case
			topY = x;
		}
		else // top < 1
		{
			// get the tile that the top vector enters from the left. since our coordinates refer to the centre of the tile,
			// this is (x-0.5)*top+0.5, which can be computed as (x-0.5)*top+0.5 = (2(x+0.5)*top+1)/2 = ((2x+1)*top+1)/2.
			// since top == a/b, this is ((2x+1)*a+b)/2b. if it enters a tile at one of the left corners,
			// it will round up, so it'll enter from the bottom-left and never the top-left
			topY = ((x * 2 - 1) * top.Y + top.X) / (top.X * 2); // the Y coordinate of the tile entered from the left
			// now it's possible that the vector passes from the left side of the tile up into the tile above before exiting from the right side of this column.
			// so we may need to increment topY
			if (BlocksLightRef(x, topY, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap)) // if the tile blocks light (i.e. is a wall)...
			{
				// if the tile entered from the left blocks light, whether it passes into the tile above depends on the shape of the wall tile as well as the angle of the vector.
				// if the tile has does not have a bevelled top-left corner, then it is blocked.
				// the corner is bevelled if the tiles above and to the left are not walls.
				// we can ignore the tile to the left because if it was a wall tile, the top vector must have entered this tile from the bottom-left corner,
				// in which case it can't possibly enter the tile above.
				//
				// otherwise, with a bevelled top-left corner, the slope of the vector must be greater than or equal to the
				// slope of the vector to the top centre of the tile (x*2, topY*2+1) in order for it to miss the wall and pass into the tile above
				if (top.GreaterOrEqual(topY * 2 + 1, x * 2) && !BlocksLightRef(x, topY + 1, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap))
					topY++;
			}
			else // the tile doesn't block light
			{
				// since this tile doesn't block light, there's nothing to stop it from passing into the tile above,
				// and it does so if the vector is greater than the vector for the bottom-right corner of the tile above.
				// however, there is one additional consideration.
				// later code in this method assumes that if a tile blocks light then it must be visible,
				// so if the tile above blocks light we have to make sure the light actually impacts the wall shape.
				// now there are three cases:
				// 1) the tile above is clear,
				//    in which case the vector must be above the bottom-right corner of the tile above
				// 2) the tile above blocks light and does not have a bevelled bottom-right corner,
				//    in which case the vector must be above the bottom-right corner
				// 3) the tile above blocks light and does have a bevelled bottom-right corner,
				//    in which case the vector must be above the bottom centre of the tile above (i.e. the corner of the bevelled edge).
				//
				// now it's possible to merge 1 and 2 into a single check, and we get the following:
				// if the tile above and to the right is a wall, then the vector must be above the bottom-right corner.
				// otherwise, the vector must be above the bottom centre.
				// this works because if the tile above and to the right is a wall, then there are two cases:
				// 1) the tile above is also a wall, in which case we must check against the bottom-right corner,
				// or 2) the tile above is not a wall, in which case the vector passes into it if it's above the bottom-right corner.
				// so either way we use the bottom-right corner in that case.
				// now, if the tile above and to the right is not a wall, then we again have two cases:
				// 1) the tile above is a wall with a bevelled edge, in which case we must check against the bottom centre,
				// or 2) the tile above is not a wall, in which case it will only be visible if light passes through the inner square,
				// and the inner square is guaranteed to be no larger than a wall diamond, so if it wouldn't pass through a wall diamond then it can't be visible,
				// so there's no point in incrementing topY even if light passes through the corner of the tile above.
				// so we might as well use the bottom centre for both cases.
				int ax = x * 2; // centre
				if (BlocksLightRef(x + 1, topY + 1, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap))
					ax++; // use bottom-right if the tile above and right is a wall
				if (top.Greater(topY * 2 + 1, ax))
					topY++;
			}
		}

		int bottomY;
		if (bottom.Y == 0) // if bottom == 0/?, then it's hitting the tile at Y=0 dead centre. this is special-cased because
		{                  // bottom.Y starts at zero and remains zero as long as it doesn't hit anything, so it's common
			bottomY = 0;
		}
		else // bottom > 0
		{
			bottomY = ((x * 2 - 1) * bottom.Y + bottom.X) / (bottom.X * 2); // the tile that the bottom vector enters from the left
			// code below assumes that if a tile is a wall then it's visible,
			// so if the tile contains a wall we have to ensure that the bottom vector actually hits the wall shape.
			// it misses the wall shape if the top-left corner is bevelled and bottom >= (bottomY*2+1)/(x*2).
			// finally, the top-left corner is bevelled if the tiles to the left and above are clear.
			// we can assume the tile to the left is clear because otherwise the bottom vector would be greater, so we only have to check above
			if (bottom.GreaterOrEqual(bottomY * 2 + 1, x * 2)
				&& BlocksLightRef(x, bottomY, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap)
				&& !BlocksLightRef(x, bottomY + 1, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap))
			{
				bottomY++;
			}
		}

		// go through the tiles in the column now that we know which ones could possibly be visible
		int wasOpaque = -1; // 0:false, 1:true, -1:not applicable
		for (int y = topY; y >= bottomY; y--) // use a signed comparison because y can wrap around when decremented
		{
			if (rangeLimit < 0 || std::invoke(GetDistance, rMap, x, y) <= rangeLimit) // skip the tile if it's out of visual range
			{
                const bool isOpaque = BlocksLightRef(x, y, octant, rMapOrigin, facingAngle, halfAngleLimit, rMap);
				// every tile where topY > y > bottomY is guaranteed to be visible.
				// also, the code that initialises topY and bottomY guarantees that if the tile is opaque then it's visible.
				// so we only have to do extra work for the case where the tile is clear and y == topY or y == bottomY.
				// if y == topY then we have to make sure that the top vector is above the bottom-right corner of the inner square.
				// if y == bottomY then we have to make sure that the bottom vector is below the top-left corner of the inner square

				// CODEPOKE adjustment for full symmetry
				// boolean isVisible = isOpaque || ((y != topY || top.greater(y * 4 - 1, x * 4 + 1)) && (y != bottomY || bottom.less(y * 4 + 1, x * 4 - 1)));
				bool isVisible = (y != topY || top.GreaterOrEqual(y, x)) && (y != bottomY || bottom.LessOrEqual(y, x));

                // NOTE: if you want the algorithm to be either fully or mostly symmetrical, replace the line above with the following line (and uncomment the LineOfSight_Slope.LessOrEqual method).
				// the line ensures that a clear tile is visible only if there's an unobstructed line to its centre.
				// if you want it to be fully symmetrical, also remove the "isOpaque ||" part and see NOTE comments further down
				// bool isVisible = isOpaque || ((y != topY || top.GreaterOrEqual(y, x)) && (y != bottomY || bottom.LessOrEqual(y, x)));

				if (isVisible)
					SetVisibleRef(x, y, octant, rMapOrigin, facingAngle, halfAngleLimit, rVisibleLocations, rMap);

				// if we found a transition from clear to opaque or vice versa, adjust the top and bottom vectors
				if (x != rangeLimit) // but don't bother adjusting them if this is the last column anyway
				{
					if (isOpaque) {
						if (wasOpaque == 0) // if we found a transition from clear to opaque, this sector is done in this column,
						{                   // so adjust the bottom vector upward and continue processing it in the next column
                            // if the opaque tile has a bevelled top-left corner, move the bottom vector up to the top centre.
							// otherwise, move it up to the top left.
							// the corner is bevelled if the tiles above and to the left are clear.
							// we can assume the tile to the left is clear because otherwise the vector would be higher, so we only have to check the tile above
						    const int nx = x * 2, ny = y * 2 + 1; // top centre by default

							// NOTE: if you're using full symmetry and want more expansive walls (recommended), comment out the next line
							// CODEPOKE adjustment for full symmetry
							// if(blocksLight(x, y + 1, octant, mapOrigin, facingAngle, halfAngleLimit))
							    // nx--; // top left if the corner is not bevelled

							if (top.Greater(ny, nx)) // we have to maintain the invariant that top > bottom, so the new sector
							{							 // created by adjusting the bottom is only valid if that's the case
								// if we're at the bottom of the column, then just adjust the current sector rather than recursing
								// since there's no chance that this sector can be split in two by a later transition back to clear
								if (y == bottomY) {
                                    const LineOfSight_Slope newBottom(ny, nx);
									bottom = newBottom;
									break;
								} // don't recurse unless necessary
								
								LineOfSight_Slope newBottom(ny, nx);
								Compute(octant, rMapOrigin, rangeLimit, facingAngle, halfAngleLimit, x + 1, top, newBottom, rVisibleLocations, rMap);
							}
							else // the new bottom is greater than or equal to the top, so the new sector is empty and we'll ignore it.
							{    // if we're at the bottom of the column, we'd normally adjust the current sector rather than recursing
								if (y == bottomY)
									return; // so that invalidates the current sector and we're done
							}
						}
						wasOpaque = 1;
					}
					else {
						if (wasOpaque > 0) // if we found a transition from opaque to clear, adjust the top vector downwards
						{
							// if the opaque tile has a bevelled bottom-right corner, move the top vector down to the bottom centre.
							// otherwise, move it down to the bottom right.
							// the corner is bevelled if the tiles below and to the right are clear.
							// we know the tile below is clear because that's the current tile, so just check to the right
                            const int nx = x * 2, ny = y * 2 + 1; // the bottom of the opaque tile (oy*2-1) equals the top of this tile (y*2+1)

						    // NOTE: if you're using full symmetry and want more expansive walls (recommended), comment out the next line
							// CODEPOKE adjustment for full symmetry
							// if(blocksLight(x + 1, y + 1, octant, mapOrigin, facingAngle, halfAngleLimit))
								// nx++; // check the right of the opaque tile (y+1), not this one

						    // we have to maintain the invariant that top > bottom. if not, the sector is empty and we're done
							if (bottom.GreaterOrEqual(ny, nx))
								return;
                            const LineOfSight_Slope newTop(ny, nx);
							top = newTop;
						}
						wasOpaque = 0;
					}
				}
			}
		}

		// if the column didn't end in a clear tile, then there's no reason to continue processing the current sector because that means either
		// 1) wasOpaque == -1, implying that the sector is empty or at its range limit,
		// or 2) wasOpaque == 1, implying that we found a transition from clear to opaque and we recursed and we never found a transition back to clear,
		// so there's nothing else for us to do that the recursive method hasn't already.
		// (if we didn't recurse (because y == bottomY), it would have executed a break, leaving wasOpaque equal to 0.)
		if (wasOpaque != 0)
			break;
	}
}

// CODEPOKE Calculate whether we go OoB on angle
bool LineOfSight::IsAngleOutOfBounds(int x, int y, const MapLocation& rMapOrigin, float facingAngle, float halfAngleLimit) const
{
	// If half of the Unit's limit is 180, they have full 360-vision, so nothing is out of bounds;
	if (halfAngleLimit >= FULL_ANGLE_LIMIT)
		return false;

    const std::pair angleToTarget(static_cast<float>(x - rMapOrigin.GetX()), static_cast<float>(y - rMapOrigin.GetY()));
	float angle = VectorHelpFunctions::AngleDeg(angleToTarget);
	float delta = std::abs(facingAngle - angle);

	bool oob = delta > halfAngleLimit && delta < 360 - halfAngleLimit;
	if (DEBUG)
        std::cout << std::format("Origin {0:s} | Location [{1:d},{2:d}] | Angle {3:4.2f} | Facing {4:4.2f} | Half-Limit {5:4.2f} | Delta {6:4.2f} | OoB {7} \n",
	    rMapOrigin.ToString(), x, y, angle, facingAngle, halfAngleLimit, delta, oob);

	return oob;
}

bool LineOfSight::BlocksLightRef(const int x, const int y, const int octant, const MapLocation& rMapOrigin, const float facingAngle, const float halfAngleLimit, HunterKillerMap& rMap) {
	int nx = rMapOrigin.GetX(), ny = rMapOrigin.GetY();
	TranslateOctantCoordsToMapCoords(x, y, octant, nx, ny);

	// CODEPOKE Check angle limit
	if (IsAngleOutOfBounds(nx, ny, rMapOrigin, facingAngle, halfAngleLimit)) {
		return true;
	}

	return std::invoke(BlocksLight, rMap, nx, ny);
}

void LineOfSight::SetVisibleRef(const int x, const int y, const int octant, const MapLocation& rMapOrigin, const float facingAngle, const float halfAngleLimit, std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations, const HunterKillerMap& rMap) const
{
	int nx = rMapOrigin.GetX(), ny = rMapOrigin.GetY();
	TranslateOctantCoordsToMapCoords(x, y, octant, nx, ny);
	
	if (!rMap.IsXOnMap(nx) || !rMap.IsYOnMap(ny))
		return;

	// CODEPOKE Check angle limit
	if (IsAngleOutOfBounds(nx, ny, rMapOrigin, facingAngle, halfAngleLimit)) {
		return;
	}
	
	rVisibleLocations.emplace(rMap.ToLocation(nx, ny));
}

void LineOfSight::TranslateOctantCoordsToMapCoords(const int octantX, const int octantY, const int octant, int& rMapX, int& rMapY)
{
	switch (octant) {
	case 0:
		rMapX += octantX;
		rMapY -= octantY;
		break;
	case 1:
		rMapX += octantY;
		rMapY -= octantX;
		break;
	case 2:
		rMapX -= octantY;
		rMapY -= octantX;
		break;
	case 3:
		rMapX -= octantX;
		rMapY -= octantY;
		break;
	case 4:
		rMapX -= octantX;
		rMapY += octantY;
		break;
	case 5:
		rMapX -= octantY;
		rMapY += octantX;
		break;
	case 6:
		rMapX += octantY;
		rMapY += octantX;
		break;
	case 7:
		rMapX += octantX;
		rMapY += octantY;
		break;
	default:
		throw std::invalid_argument("Unsupported octant used as argument.");
	}
}

void LineOfSight::NextOctant(const int octant, std::pair<float, float>& vector) {
	vector.first = 1.0f;
	vector.second = 0.0f;
	VectorHelpFunctions::SetAngleDeg(vector, static_cast<float>(octant) * -45.0f);
}

float LineOfSight::MinimumAngleToOctant(const int octant, const Direction facing) {
	switch (facing) {
	case EAST:
		switch (octant) {
		case 0:
			return 0;
		case 1:
			return 45;
		case 2:
			return 90;
		case 3:
			return 135;
		case 4:
			return 135;
		case 5:
			return 90;
		case 6:
			return 45;
		case 7:
			return 0;
		default:
			throw std::invalid_argument("Unsupported octant used as argument.");
		}
	case NORTH:
		switch (octant) {
		case 0:
			return 45;
		case 1:
			return 0;
		case 2:
			return 0;
		case 3:
			return 45;
		case 4:
			return 90;
		case 5:
			return 135;
		case 6:
			return 135;
		case 7:
			return 90;
		default:
			throw std::invalid_argument("Unsupported octant used as argument.");
		}
	case SOUTH:
		switch (octant) {
		case 0:
			return 90;
		case 1:
			return 135;
		case 2:
			return 135;
		case 3:
			return 90;
		case 4:
			return 45;
		case 5:
			return 0;
		case 6:
			return 0;
		case 7:
			return 45;
		default:
			throw std::invalid_argument("Unsupported octant used as argument.");
		}
	case WEST:
		switch (octant) {
		case 0:
			return 135;
		case 1:
			return 90;
		case 2:
			return 45;
		case 3:
			return 0;
		case 4:
			return 0;
		case 5:
			return 45;
		case 6:
			return 90;
		case 7:
			return 135;
		default:
			throw std::invalid_argument("Unsupported octant used as argument.");
		}
	}
	return std::nanf("");
}

void LineOfSight::CacheVisibleLocations(const LineOfSight_CacheEntry& rEntry, const std::unordered_set<MapLocation, MapLocationHash>& rVisibleLocations) const
{
	if (!HaveCached(rEntry)) {
		(*FoVCache)[rEntry] = rVisibleLocations;
	}
}

bool LineOfSight::HaveCached(const LineOfSight_CacheEntry& rEntry) const
{
	return FoVCache->contains(rEntry);
}

std::unordered_set<MapLocation, MapLocationHash>* LineOfSight::GetFromCache(const LineOfSight_CacheEntry& rEntry) const
{
	return &(*FoVCache)[rEntry];
}

GetDistanceFunction LineOfSight::GetDistanceType() const
{
	return GetDistance;
}
