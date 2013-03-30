#include "IronClad/Math/Line2.hpp"

using namespace ic::math;

line2_t::line2_t(const vector2_t& Start, const vector2_t& End)
{
    this->Start = Start;
    this->End    = End;
}

line2_t::line2_t(
    const float start_x, const float start_y,
    const float end_x, const float end_y)
{
    Start.x    = start_x;
    Start.y    = start_y;
    End.x    = end_x;
    End.y    = end_y;
}

bool line2_t::Intersects(const line2_t& Other,
    vector2_t* pIntersection) const
{
    vector2_t Intersection;

    float m1 = this->GetSlope();
    float m2 = Other.GetSlope();

    // Test for infinite slopes or parallel lines.
    if(Start.x == End.x)
        Intersection.x = Start.x;
    else if(Other.Start.x == Other.End.x)
        Intersection.x = Other.Start.x;
    else if(m1 == m2)
        return false;
    else
        Intersection.x = (-m2 * Other.Start.x + Other.Start.y + 
            m1 * Start.x - Start.y) / (m1 - m2);

    Intersection.y = m1 * Intersection.x - m1 * Start.x + Start.y;

    if(this->OnSegment(Intersection) && Other.OnSegment(Intersection))
    {
        if(pIntersection != NULL)
            *pIntersection = Intersection;
        return true;
    }
    else
        return false;
}

bool line2_t::OnSegment(const vector2_t& Point) const
{
    // Final return var
    bool on_line = false;

    // Determine if the current ray goes left-to-right
    // (from Start to End) or right-to-left (from End to Start)
    bool ltr = (Start.x < End.x);

    // Determine if the current ray goes top-to-bottom
    // (from Start to End) or bottom-to-top (from End to Start)
    bool ttb = (Start.y < End.y);

    // Check x range
    on_line = ltr ? 
        Point.x >= Start.x && 
        Point.x <= End.x : 
        Point.x >= End.x && 
        Point.x <= Start.x;

    if(!on_line)
        return false;
    else
        return (ttb ? 
            Point.y >= Start.y && 
            Point.y <= End.y : 
            Point.y >= End.y && 
            Point.y <= Start.y);
}

float line2_t::GetSlope(bool* is_inf /*= NULL*/) const
{
    if(Start.x == End.x)
    {
        if(is_inf != NULL) *is_inf = true;
        return 0;
    }

    if(is_inf != NULL) *is_inf = false;
    return (End.y - Start.y) / (End.x - Start.x);
}
