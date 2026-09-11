#include <stdafx.h>

#include <vector>

#include "Inventor\SbVec.h"

// it looks like min/max being defined depends on the environment
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <algorithm>

// Given three colinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(const SbVec2d & p, const SbVec2d & q, const SbVec2d & r)
{
  if (q[0] <= std::max(p[0], r[0]) && q[0] >= std::min(p[0], r[0]) &&
      q[1] <= std::max(p[1], r[1]) && q[1] >= std::min(p[1], r[1]))
        return true;

    return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(const SbVec2d & p, const SbVec2d & q, const SbVec2d & r)
{
    // See 10th slides from following link for derivation of the formula
    // http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
    int val = (q[1] - p[1]) * (r[0] - q[0]) -
        (q[0] - p[0]) * (r[1] - q[1]);

    if (val == 0) return 0;  // colinear

    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool doIntersect(const SbVec2d & p1, const SbVec2d & q1, const SbVec2d & p2, const SbVec2d & q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1))
        return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1))
        return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2))
        return true;

    // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2))
        return true;

    return false; // Doesn't fall in any of the above cases
}

// not closed
bool selfIntersects (const std::vector<SbVec2f> & polygon)
{
    size_t size = polygon.size();
    if (size < 4) return false;

    for (int i1 = 0; i1 < size - 2; ++i1)
    {
        SbVec2d p1 (polygon[i1] * 1000);
        SbVec2d q1 (polygon[(i1 + 1) % size] * 1000);

        for (int i2 = i1 + 2; i2 < size; ++i2)
        {
            if (i1 == 0 && i2 == size - 1) continue; // first and last line of polygon are adjacent too

            SbVec2d p2 (polygon[i2] * 1000);
            SbVec2d q2 (polygon[(i2 + 1) % size] * 1000);

            if (doIntersect(p1, q1, p2, q2))
                return true;
        }
    }

    return false;
}