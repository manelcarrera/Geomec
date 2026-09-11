#ifndef _EXPORTSELECTION_H_
#define _EXPORTSELECTION_H_

#include "OpenGLNodeSelection.h"
#include "HorizonBase.h"
#include "ElementSet.h"
#include "FormationPlane.h"
#include "WellCasingRTCI.h"
#include "PointSet.h"
#include "NewWellPath.h"
#include "FormationBase.h"

class CHorizonBase;
class IElementSet;
class CFormationPlane;
class CWellCasingRTCI;
class CPointSet;
class CNewWellPath;
class CFormationBase;
class CNonMeshedSurface;

typedef CTypedSelection<CHorizonBase> THorizonBaseSelection;
typedef CTypedSelection<CFormationBase> TFormationSelection;
typedef CTypedSelection<CFormationPlane> TFormationPlaneSelection;
typedef CTypedSelection<CWellCasingRTCI> TRTCISelection;
typedef CTypedSelection<CNewWellPath> TNewWellPathSelection;
typedef CTypedSelection<CPointSet> TPointSetSelection;
typedef CTypedSelection<IElementSet> TElementSetSelection;
typedef CTypedSelection<CNonMeshedSurface> TNonMeshedSurfaceSelection;

class CSlipHorizonSelectionFilter : public THorizonBaseSelection::CFilter<CHorizonBase>
{
public:
  virtual bool operator()(const CHorizonBase& hor) const;
};

class CHorizonSelectionFilter : public THorizonBaseSelection::CFilter<CHorizonBase>
{
public:
  virtual bool operator()(const CHorizonBase& hor) const;
};

class CElementSetSelectionFilter : public TElementSetSelection::CFilter<IElementSet>
{
public:
  virtual bool operator()(const IElementSet& elset) const;
};

#endif // _EXPORTSELECTION_H_
