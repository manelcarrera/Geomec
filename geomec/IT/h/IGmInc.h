#pragma once

static const double NaN = -9999.999; // FIXME

namespace geo {
class IPoint;
class CValue;
class IElement;
} // namespace geo

// for displacements
typedef std::pair<geo::IPoint &, geo::CValue> TPointValueP;
typedef std::vector<TPointValueP> TPointValueV;

typedef std::list<std::string> TStringL;

// for pressures
typedef std::vector<geo::CValue> TValuesV;
typedef std::pair<geo::IElement &, TValuesV> TElemValuesP;
typedef std::vector<TElemValuesP> TElemValuesV;

// general
typedef std::vector<geo::IPoint &> TPointsV;
typedef std::vector<geo::IElement &> TElemsV;

typedef std::vector<geo::IPoint &> TPoint;

class CStringV;
class IValueComposite;
class CPointSet;

typedef CStringV TReservoirL;
typedef CStringV TDepletionL;
typedef CStringV TDisplacementL;
typedef CStringV TSurfaceL;
typedef std::vector<IValueComposite *> TCompositeV;
typedef std::vector<CPointSet *> TPointSetV;