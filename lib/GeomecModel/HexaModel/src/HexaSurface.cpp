// HexaSurface.cpp: implementation of the CHexaSurface class.
//
//////////////////////////////////////////////////////////////////////

#include "HexaSurface.h"
#include "HexaHorizon.h" 
#include "HexaEntryTypes.h" 
#include "FemAppModel.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHexaSurface::CHexaSurface(CFemAppModel& model)
: CSurfaceBase(model)
{
}

CHexaSurface::CHexaSurface(CSurfaceValueType &value_type)
: CSurfaceBase(value_type)
{
}

CHexaSurface::CHexaSurface(const QString &strName, const geo::ISurface& surface, CFemAppModel& model)
: CSurfaceBase(strName, surface, model)
{
}

CHexaSurface::CHexaSurface(const QString &strName, const geo::CPtrArray<geo::IPoint> &vcPoint, CFemAppModel& model)
: CSurfaceBase(strName, vcPoint, model)
{
}

CHexaSurface::CHexaSurface(const QString &strName, const geo::CArray<geo::CPoint> &vcPoint, CFemAppModel& model)
: CSurfaceBase(strName, vcPoint, model)
{
}

void CHexaSurface::CreateHorizon()
{
	CHexaHorizonEntry* pEntry = (CHexaHorizonEntry*)(Model().GraphEntry(MD_HEXA_HORIZON));
	pEntry->ConnectItem(*this);
}

CSurfaceBase* CHexaSurface::OnCreateSurface(const QString& sName, const geo::CPtrArray<geo::IPoint>& vcPoint)
{
	return new CHexaSurface(sName, vcPoint, Model());
}