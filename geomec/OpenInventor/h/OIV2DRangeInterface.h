#ifndef _OIV2D_RANGE_INTERFACE_
#define _OIV2D_RANGE_INTERFACE_

class OIV2DGraph;


class OIV2DRangeInterface
{
public:
  typedef enum { AXIS_LEFT = 0, AXIS_TOP, AXIS_BOTTOM, _AXIS_TOTAL } TAxis;

  OIV2DRangeInterface() {}
  virtual ~OIV2DRangeInterface() {}

  virtual void Enter() = 0;
  virtual void Leave(bool revert = false) = 0;

  virtual bool HaveAxis(TAxis axis) const = 0;

  virtual void GetRangeMinMax(TAxis axis, float& min, float& max) const = 0;
  virtual bool RangeLargeEnough(TAxis axis, float min, float max) const = 0;
  virtual void SetMinMax(TAxis axis, float min, float max) = 0;

  virtual void ResetMinMax(TAxis axis) = 0;

  virtual int Tick(TAxis axis) const = 0;
  virtual void Tick(TAxis axis, int value) = 0;

  virtual int SubTick(TAxis axis) const = 0;
  virtual void SubTick(TAxis axis, int value) = 0;

  virtual bool Auto(TAxis axis) const = 0;
  virtual void Auto(TAxis axis, bool value) = 0;

  virtual void UpdateGraph() = 0;
};



#endif