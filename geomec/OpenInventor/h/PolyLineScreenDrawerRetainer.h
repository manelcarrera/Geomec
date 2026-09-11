#ifndef _PolyLineScreenDrawerRetainer_h_
#define _PolyLineScreenDrawerRetainer_h_

class SbVec2f;

class SoPolyLineScreenDrawer;

class COpenInventorEventsHandler;

/// Class to contain real world coordinates for a polylinedrawer during camera manipulation
/// As the real world coordinates are stored in the polylinedrawer we can only effectively handle one polylinedrawer at a time and only one retainer per polylinedrawer at a time.
/// So, there will always be at most one PolyLineDrawer that "CanWork()".
/// Requires to be in TopView mode in order to "CanWork()" as functions used work only for TopView mode.
class PolyLineScreenDrawerRetainer
{
public:
    PolyLineScreenDrawerRetainer(COpenInventorEventsHandler * eventHandler, SoPolyLineScreenDrawer * drawer);

    bool CanWork();

    ~PolyLineScreenDrawerRetainer();

private:
    static PolyLineScreenDrawerRetainer * s_activeRetainer;
    COpenInventorEventsHandler * m_handler;
    SoPolyLineScreenDrawer * m_drawer;
    SbVec2f * m_points;
};

#endif  // _PolyLineScreenDrawerRetainer_h_
