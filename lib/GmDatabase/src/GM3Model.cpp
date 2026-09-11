// 3DModel.cpp: implementation of the C3DModel class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GM3Model.h"
#include "GM3WellPoint.h"
#include "GM3Database.h"
#include "GM3TableDef.h"
#include "GM3StringTable.H"
#include "GM3IncompleteException.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace gm {

//##ModelId=3BC55D4D00E5
C3DModel::C3DModel()
: m_bTimeAnalysis(TRUE)
{
}

//##ModelId=3BC55D4D00F0
C3DModel::~C3DModel()
{
}

//##ModelId=3BC55D4D00D2
const std::vector<C3DHorizon>& C3DModel::Horizons() const
{
	return m_vcHorizon;
}

//##ModelId=3BC55D4D00D4
const std::vector<C3DFormation>& C3DModel::Formations() const
{
	return m_vcFormation;
}

const std::vector<C3DCrossSection>& C3DModel::CrossSections() const
{
	return m_vcCrossSection;
}

const std::vector<C3DWellPath>& C3DModel::WellPaths() const
{
	return m_vcWellPath;
}

//##ModelId=3BC55D4D00C4
#ifndef _WIN64
void C3DModel::LoadModel(const CString &strPath, const BOOL bComplete, IProgressBase &callback)
{
	// Create and open the database ...
	C3DDatabase db;
	db.OpenDB(strPath, bComplete);

	// Get the total number of records ...
	callback.AddSteps(2);	// Global settings and mesh
	callback.AddSteps(db.TableSize(TB_TIMESTEP));
	callback.AddSteps(db.TableSize(TB_HORIZON));
	callback.AddSteps(db.TableSize(TB_HORIZON_PT));
	callback.AddSteps(db.TableSize(TB_FORMATION));
	callback.AddSteps(db.TableSize(TB_PRESSURE));
	callback.AddSteps(db.TableSize(TB_PRESSURE_PT));
	callback.AddSteps(db.TableSize(TB_MESH_ZONE));
	callback.AddSteps(db.TableSize(TB_MESH_ZONE_NODE));


	// Read global settings
	ReadGlobalSettings(db, callback);

	// Read Timesteps
	ReadTimeSteps(db, callback);

	// Read Horizons ...
	CDaoRecordset rs(&db);
	CString strQuery = _T("]");
	strQuery += TB_HORIZON;
	strQuery += _T("]");
	
	rs.Open(dbOpenTable, strQuery,dbReadOnly);
	int nIndex = 0;
	if (!rs.IsBOF())
	{
		rs.MoveLast();
		rs.MoveFirst();

		m_vcHorizon.resize(rs.GetRecordCount());

		while (!rs.IsEOF())
		{
			// Update on every new horzion
			OnNewTable(CString(IDS_HORIZON) + rs.GetFieldValue(FD_HORIZON_NAME).pcVal);
			
			COleVariant vFieldContents;
			vFieldContents = rs.GetFieldValue(FD_HORIZON_NR);

			m_vcHorizon[nIndex].LoadHorizon(vFieldContents.lVal, db, callback);
			rs.MoveNext();

			nIndex++;
		}
	}

	rs.Close();
	
	//Read formation table
	strQuery = _T("]");
	strQuery += TB_FORMATION;
	strQuery += _T("]");
	
	rs.Open(dbOpenTable, strQuery,dbReadOnly);

	if (!rs.IsBOF())
	{
		rs.MoveLast(); //Needed for accurate GetRecordCount() !!
		rs.MoveFirst();

		m_vcFormation.resize(rs.GetRecordCount());
		nIndex=0;
		while (!rs.IsEOF())
		{
			// Update on every new formation
			OnNewTable(CString(IDS_FORMATION) + rs.GetFieldValue(FD_FORMATION_NAME).pcVal);

			COleVariant vFieldContents;
			vFieldContents=rs.GetFieldValue("FormationNr");

			m_vcFormation[nIndex].ReadFormation(db, *this, vFieldContents.lVal, callback);
			nIndex++;
			rs.MoveNext();
		}
	}
	rs.Close();
	
	// Read mesh table
	ReadMesh(db, callback);

	// Read Cross Sections ...
	strQuery = _T("[");
	strQuery += TB_XSECTION;
	strQuery += _T("]");
	
	rs.Open(dbOpenTable, strQuery,dbReadOnly);

	// Does the recordset contain records?
	if(!rs.IsBOF())
	{
		rs.MoveLast();
		rs.MoveFirst();

		m_vcCrossSection.resize(rs.GetRecordCount());
		nIndex = 0;

		while (!rs.IsEOF())
		{
			// Update on every new horzion
			OnNewTable(CString(IDS_XSECTION) + rs.GetFieldValue(FD_XSECTION_NAME).pcVal);
			
			COleVariant vFieldContents;
			vFieldContents = rs.GetFieldValue(FD_XSECTION_INDEX);

			m_vcCrossSection[nIndex].ReadCrossSection(db, vFieldContents.lVal, callback);
			rs.MoveNext();

			nIndex++;
		}
	}

	rs.Close();

	// Read WellPaths...
	strQuery = _T("[");
	strQuery += TB_LINER;
	strQuery += _T("]");
	
	rs.Open(dbOpenTable, strQuery,dbReadOnly);

	// Does the recordset contain records?
	if(!rs.IsBOF())
	{
		rs.MoveLast();
		rs.MoveFirst();

		m_vcWellPath.resize(rs.GetRecordCount());
		nIndex = 0;

		while (!rs.IsEOF())
		{
			// Update on every new horzion
			OnNewTable(CString(IDS_WELLPATH) + rs.GetFieldValue(FD_LINER_NAME).pcVal);
			
			COleVariant vFieldContents;
			vFieldContents = rs.GetFieldValue(FD_LINER_INDEX);

			m_vcWellPath[nIndex].ReadWellPath(db, vFieldContents.lVal, callback);
			rs.MoveNext();

			nIndex++;
		}
	}

	rs.Close();
}
#endif

void C3DModel::ReadMesh(C3DDatabase &db, IProgressBase &callback)
{
#ifndef _WIN64
	// Open the mesh table ...
	CDaoRecordset rs(&db);
	CString strQuery = _T("[");
	strQuery +=  TB_MESH;
	strQuery += "]";
	rs.Open(dbOpenTable, strQuery, dbReadOnly);

	if(rs.IsEOF()) 
	{
		// Throw incomplete model exception ...
		throw new CIncompleteException();
	}

	// Move to first and update progress counter ...
	rs.MoveFirst();
	callback.Step();

	// Available in every version
	m_dRectangleSize	= rs.GetFieldValue(FD_MESH_DELTA_XY).dblVal;

	m_min = geo::CPoint( rs.GetFieldValue(FD_MESH_MIN_Y).dblVal,
						 rs.GetFieldValue(FD_MESH_MIN_X).dblVal);

	m_max = geo::CPoint( rs.GetFieldValue(FD_MESH_MAX_Y).dblVal,
						 rs.GetFieldValue(FD_MESH_MAX_X).dblVal);

	rs.Close();

	//Read mesh zone table
	strQuery = _T("]");
	strQuery += TB_MESH_ZONE;
	strQuery += _T("]");
	
	rs.Open(dbOpenTable, strQuery,dbReadOnly);

	// Does the recordset contain records?
	if(!rs.IsBOF())
	{
		rs.MoveLast(); //Needed for accurate GetRecordCount() !!
		rs.MoveFirst();

		m_vcMeshZone.resize(rs.GetRecordCount());
		int nIndex=0;
		while (!rs.IsEOF())
		{
			// Update on every new formation
		//		callback.OnNewTable(CString(IDS_FORMATION) + rs.GetFieldValue(FD_FORMATION_NAME).pcVal);

			COleVariant vFieldContents;
			vFieldContents=rs.GetFieldValue(FD_MESH_ZONE_ID);

			m_vcMeshZone[nIndex].ReadMeshZone(db, *this, vFieldContents.lVal, callback);
			nIndex++;
			rs.MoveNext();
		}
	}

	rs.Close();
#endif
}

//##ModelId=3BC55D4D00F2
void C3DModel::ReadGlobalSettings(C3DDatabase &db, IProgressBase &callback)
{
#ifndef _WIN64
	// Open the globals table ...
	CDaoRecordset rs(&db);
	CString strQuery = _T("[");
	strQuery +=  TB_GLOBAL_SETTINGS;
	strQuery += "]";
	rs.Open(dbOpenTable, strQuery, dbReadOnly);

	if(rs.IsEOF()) 
	{
		// Throw incomplete model exception ...
		throw new CIncompleteException();
	}

	// Move to first and update progress counter ...
	rs.MoveFirst();
	callback.Step();

	// Read: Importing eclipse tag ... else there is no eclipse
	if(db.Version() >= 2.0) 
		m_bEclipseFile = rs.GetFieldValue("ImportingEclipse").boolVal;
	else 
		m_bEclipseFile = false;
	

	// Available in every version
	m_dTotalVerticalStressGradient	= rs.GetFieldValue(FD_TOT_VERT_STRESS_GRAD).dblVal;
	m_dVerticalStressExponent		= rs.GetFieldValue(FD_VERT_STRESS_EXP).dblVal;
	m_dWaterColumnStressGradient	= rs.GetFieldValue(FD_WATER_COL_GRAD).dblVal;
	m_strProjectInfo				= rs.GetFieldValue(FD_PROJECT_INFO).pcVal;
	if(db.FieldExist(TB_GLOBAL_SETTINGS, FD_TIME_ANALYSIS))
	{
		m_bTimeAnalysis				= rs.GetFieldValue(FD_TIME_ANALYSIS).boolVal;
	}

	// Resize timesteps
	m_vcTimestep.resize( rs.GetFieldValue(FD_NR_OF_TIMESTEPS).iVal + 1 );

	rs.Close();
#endif
}

int C3DModel::TimestepSize() const
{
	return m_vcTimestep.size();
}

const CTimestep& C3DModel::Timestep(int nIndex) const
{
	return m_vcTimestep[nIndex];
}

// Pre	: Timesteps vector has been sized ...
// Post : Timesteps vecotr is initialised ...
//##ModelId=3BC55D4D00FF
void C3DModel::ReadTimeSteps(C3DDatabase &db, IProgressBase &callback)
{
#ifndef _WIN64
	// Update progress
	CString strMessage = IDS_TIMESTEPS;
	OnNewTable(strMessage);

	// Initialize query string ...
	CDaoRecordset rs(&db);
	CString strQuery = CString(_T("SELECT ")) + CString(FD_TIMESTEP_NAME);
	
	if(db.Version() >= 2.6)
		strQuery+= CString(", ") + CString(FD_YEAR)  + CString(", ") + FD_YEAROK;	// Additional 2.6 fields

	strQuery += CString(_T(" FROM ")) + CString(TB_TIMESTEP) + CString(_T(" ORDER BY ")) + FD_TIMESTEP_NR + " ASC";

	// Open the recordset ...
	rs.Open(dbOpenDynaset,strQuery,dbReadOnly);

	if(rs.IsEOF()) 
	{
		// Throw incomplete model exception ...
		throw new CIncompleteException();
	}

	rs.MoveFirst();

	// From version 2.6 the initial step is included in the database and
	// the year field is added ...
	int nTimeStep = 0;

	if(db.Version() >= 2.6) 
		nTimeStep = -1;

	while(!rs.IsEOF()) 
	{
		// Read time step ...
		callback.Step();
		m_vcTimestep[nTimeStep + 1].ReadRecord(rs, db.Version());

		if(db.Complete() && (!m_vcTimestep[nTimeStep].Year().Valid())) 
		{
			// Throw incomplete model exception ...
			throw new CIncompleteException();
		}

		nTimeStep++;
		rs.MoveNext();
	}

	rs.Close();
	
	m_vcTimestep[0] = C3DTimestep(IDS_INITIAL_TIMESTEP_NAME);
	
	if(db.Version() >= 2.2)
	{
		//Read NONLIN table
		strQuery = _T("]");
		strQuery += TB_NONLIN;
		strQuery += _T("]");
		
		rs.Open(dbOpenTable, strQuery,dbReadOnly);

		// Does the recordset contain records?
		if(!rs.IsBOF())
		{
			rs.MoveLast(); //Needed for accurate GetRecordCount() !!
			rs.MoveFirst();

			assert(m_vcTimestep.size() == rs.GetRecordCount() + 1);
			int nIndex=1;
			while (!rs.IsEOF())
			{
				m_vcTimestep[nIndex].ReadNonLinRecord(rs);
				// Update on every new formation
				nIndex++;
				rs.MoveNext();
			}
		}

		rs.Close();
	}
#endif	
}

const geo::IPoint& C3DModel::Min() const
{
	return m_min;
}

const geo::IPoint& C3DModel::Max() const
{
	return m_max;
}

const double& C3DModel::RectangleSize() const
{
	return m_dRectangleSize;
}

const std::vector<C3DMeshZone>& C3DModel::MeshZones() const
{
	return m_vcMeshZone;
}

} // End of gm3 namespace