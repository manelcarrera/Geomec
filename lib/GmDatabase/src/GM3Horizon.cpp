// Horizon.cpp: implementation of the C3DHorizon class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GM3Database.h"
#include "GM3Horizon.h"
#include "GM3TableDef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm {
//##ModelId=3BA8A1C801D9
C3DHorizon::C3DHorizon()
{

}

//##ModelId=3BA8A1C801DA
C3DHorizon::~C3DHorizon()
{

}

//##ModelId=3BA8A1C801CE
const QString& C3DHorizon::Name() const
{
	return m_strName;
}

//##ModelId=3BA8A1C801CB
void C3DHorizon::LoadHorizon(const long nIndex, C3DDatabase &db, IProgressBase &callback)
{
#ifndef _WIN64
	// Horizon data ...
	CDaoRecordset rs(&db);
	QString strQuery = _T("SELECT * FROM ");
	strQuery += TB_HORIZON;
	strQuery += _T(" WHERE ");
	strQuery += FD_HORIZON_NR;
	strQuery += _T("=%1");
	QString strNewQuery;
	strNewQuery = QString(strQuery).arg(nIndex);
	rs.Open(dbOpenDynaset,strNewQuery.toStdString().c_str(),dbReadOnly);

		
	rs.MoveFirst();
	callback.Step();
	m_strName = rs.GetFieldValue(FD_HORIZON_NAME).pcVal; 
	m_lColor=rs.GetFieldValue(FD_HORIZON_COLOR).lVal;
	m_bConstantDepth=rs.GetFieldValue(FD_HORIZON_CONST_DEPTH).boolVal;

	m_ptMin = geo::CPoint( rs.GetFieldValue(FD_HORIZON_Y_MIN).dblVal,
						   rs.GetFieldValue(FD_HORIZON_X_MIN).dblVal,
						   rs.GetFieldValue(FD_HORIZON_Z_MIN).dblVal);


	m_ptMax = geo::CPoint( rs.GetFieldValue(FD_HORIZON_Y_MAX).dblVal,
						   rs.GetFieldValue(FD_HORIZON_X_MAX).dblVal,
						   rs.GetFieldValue(FD_HORIZON_Z_MAX).dblVal);
	rs.Close();


	strQuery = _T("SELECT ");
	strQuery += FD_HORIZON_PT_X;
	strQuery += _T(", ");
	strQuery += FD_HORIZON_PT_Y;
	strQuery += _T(", ");
	strQuery += FD_HORIZON_PT_Z;
	strQuery += _T(" FROM ");
	strQuery += TB_HORIZON_PT;
	strQuery += _T(" WHERE ");
	strQuery += FD_HORIZON_INDEX;
	strQuery += _T("=%1");

	strNewQuery = QString(strQuery).arg(nIndex);
	rs.Open(dbOpenDynaset, strNewQuery.toStdString().c_str(),dbReadOnly);
	rs.MoveFirst();
	
	if(m_bConstantDepth)
	{
		//Construct points for this gives better visual feedback
		for(int x = 0; x < 10; x++)
		{
			for(int y = 0; y < 10; y++)
			{
				m_vcPoint.PushBack(geo::CPoint( m_ptMin.X() + (m_ptMax.X() - m_ptMin.X())*((double)x/9.0),
												m_ptMin.Y() + (m_ptMax.Y() - m_ptMin.Y())*((double)y/9.0),
												m_ptMax.Z() ));
			}
		}
	}
	else
	{
		while(!rs.IsEOF())
		{
			callback.Step();
			COleVariant vVariant;
			vVariant=rs.GetFieldValue("X");
			m_vcPoint.PushBack(geo::CPoint( rs.GetFieldValue(FD_HORIZON_PT_Y).lVal,
											rs.GetFieldValue(FD_HORIZON_PT_X).lVal,
											rs.GetFieldValue(FD_HORIZON_PT_Z).lVal));
			rs.MoveNext();
		}
	}

	rs.Close();
#endif
}

//##ModelId=3BA8A1C801C9
BOOL C3DHorizon::ConstantDepth() const
{
	return m_bConstantDepth;
}
const C3DHorizon::TPointVec& C3DHorizon::Vertices() const
{
	return m_vcPoint;
}

const geo::IPoint& C3DHorizon::Min() const
{
	return m_ptMin;
}

const geo::IPoint& C3DHorizon::Max() const
{
	return m_ptMax;
}

int C3DHorizon::Color() const
{
	return m_lColor;
}

} // End of namespace gm3 ...



