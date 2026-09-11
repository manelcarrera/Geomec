// DaoWellInterface.cpp: implementation of the CDaoWellInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef _WIN64

#include "DaoWellInterface.h"
#include "TableWellpathGeometry.h"
#include "VersionTable.h"
#include "wellpathbase.h"
// #include <qmemarray.h>

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

#define WELLSCHEMELIB_VERSION_NAME "WellSchemeLibrary"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDaoWellInterface::CDaoWellInterface(well::CWellPathBase &wellpath) { m_pWellPath = &wellpath; }

CDaoWellInterface::~CDaoWellInterface() {}

bool CDaoWellInterface::LoadFromDatabase(const CString &filename) {

  CDaoDatabase database;
  database.Open(filename, FALSE, TRUE);

  CVersionTable version_table(database, WELLSCHEMELIB_VERSION_NAME);

  TableWellpathGeometry table(database);
  table.Open();

  QVector<double> tmd;
  QVector<double> azi;
  QVector<double> inc;

  int count = 0; // point count

  // load definition points
  if (!table.RS().IsBOF()) {
    table.RS().MoveFirst();

    while (!table.RS().IsEOF()) {
      tmd.resize(count + 1);
      azi.resize(count + 1);
      inc.resize(count + 1);
      table.GetField(WELLPATH_GEO_TMD, tmd[count]);
      table.GetField(WELLPATH_GEO_AZI, azi[count]);
      table.GetField(WELLPATH_GEO_INC, inc[count]);
      count++;
      table.RS().MoveNext();
    }
  }
  table.Close();

  // load global data
  TableWellpathGlobal table_global(database);
  table_global.Open();
  double glb_northing, glb_easting, glb_depth;
  double start_northing, start_easting, start_depth;
  table_global.GetField(WELLPATH_GLB_NORTHING, glb_northing);
  table_global.GetField(WELLPATH_GLB_EASTING, glb_easting);
  table_global.GetField(WELLPATH_GLB_DEPTH, glb_depth);

  table_global.GetField(WELLPATH_GLB_SEALEVEL, m_pWellPath->MeanSeaLevel());
  table_global.GetField(WELLPATH_GLB_GROUND_OR_SEABED_LEVEL, m_pWellPath->MeanGroundOrSeabedLevel());

  if (count) {
    table_global.GetField(WELLPATH_GLB_START_NORTHING, start_northing);
    table_global.GetField(WELLPATH_GLB_START_EASTING, start_easting);
    table_global.GetField(WELLPATH_GLB_START_DEPTH, start_depth);
  }

  CString description, well_name, type_name, field_name, location_name, country, customer, cust_contact, job_number;
  long day = 0;
  long month = 0;
  long year = 0;
  if (table_global.FieldExists(WELLPATH_STR_DESCRIPTION))
    table_global.GetField(WELLPATH_STR_DESCRIPTION, description);

  if (table_global.FieldExists(WELLPATH_STR_NAME))
    table_global.GetField(WELLPATH_STR_NAME, well_name);

  if (table_global.FieldExists(WELLPATH_STR_TYPE_NAME))
    table_global.GetField(WELLPATH_STR_TYPE_NAME, type_name);

  if (table_global.FieldExists(WELLPATH_STR_FIELD_NAME))
    table_global.GetField(WELLPATH_STR_FIELD_NAME, field_name);

  if (table_global.FieldExists(WELLPATH_STR_LOC_NAME))
    table_global.GetField(WELLPATH_STR_LOC_NAME, location_name);

  if (table_global.FieldExists(WELLPATH_STR_COUNTRY))
    table_global.GetField(WELLPATH_STR_COUNTRY, country);

  if (table_global.FieldExists(WELLPATH_STR_CUST_NAME))
    table_global.GetField(WELLPATH_STR_CUST_NAME, customer);

  if (table_global.FieldExists(WELLPATH_STR_CUST_CONT))
    table_global.GetField(WELLPATH_STR_CUST_CONT, cust_contact);

  if (table_global.FieldExists(WELLPATH_STR_JOB_NO))
    table_global.GetField(WELLPATH_STR_JOB_NO, job_number);

  if (table_global.FieldExists(WELLPATH_SPUDDATE_DAY))
    table_global.GetField(WELLPATH_SPUDDATE_DAY, day);

  if (table_global.FieldExists(WELLPATH_SPUDDATE_MONTH))
    table_global.GetField(WELLPATH_SPUDDATE_MONTH, month);

  if (table_global.FieldExists(WELLPATH_SPUDDATE_YEAR))
    table_global.GetField(WELLPATH_SPUDDATE_YEAR, year);

  table_global.Close();

  database.Close();

  if (count > 1) {
    // finaly contruct wellpath
    if (!m_pWellPath->InitFromArray_tmd_azi_inc(glb_northing, glb_easting, glb_depth, start_northing, start_easting,
                                                start_depth, tmd, azi, inc)) {
      return false;
    }
  }

  m_pWellPath->SetDescription((LPCSTR)description);
  m_pWellPath->SetWellName((LPCSTR)well_name);
  m_pWellPath->SetWellType((LPCSTR)type_name);
  m_pWellPath->SetFieldName((LPCSTR)field_name);
  m_pWellPath->SetLocationName((LPCSTR)location_name);
  m_pWellPath->SetCountry((LPCSTR)country);
  m_pWellPath->SetCustomerName((LPCSTR)customer);
  m_pWellPath->SetCustomerContact((LPCSTR)cust_contact);
  m_pWellPath->SetJobNumber((LPCSTR)job_number);

  if (year == 0 || month == 0 || day == 0)
    m_pWellPath->SetSpudDate(QDate().currentDate());
  else
    m_pWellPath->SetSpudDate(QDate(year, month, day));

  return true;
}

bool CDaoWellInterface::SaveToDatabase(const CString &filename) {
  CDaoDatabase database;
  database.Open(filename, FALSE, FALSE);

  TableWellpathGeometry table(database);

  // only creates if not exists
  table.Create();

  // store definition points in database
  table.Open();
  well::CWellPointList::Iterator it = m_pWellPath->DefPointList().begin();
  while (it != m_pWellPath->DefPointList().end()) {
    table.RS().AddNew();
    table.SetField(WELLPATH_GEO_TMD, (*it)->TMD());
    table.SetField(WELLPATH_GEO_AZI, (*it)->Azimuth());
    table.SetField(WELLPATH_GEO_INC, (*it)->Inclination());
    table.RS().Update();
    ++it;
  }
  table.Close();

  TableWellpathGlobal table_global(database);

  // only if not exists!!
  table_global.Create();

  // store global data
  table_global.Open();
  table_global.RS().AddNew();
  table_global.SetField(WELLPATH_GLB_NORTHING, m_pWellPath->GlobalNorthing());
  table_global.SetField(WELLPATH_GLB_EASTING, m_pWellPath->GlobalEasting());
  table_global.SetField(WELLPATH_GLB_DEPTH, m_pWellPath->GlobalTVD());

  table_global.SetField(WELLPATH_STR_DESCRIPTION, m_pWellPath->GetDescription().toAscii().constData());
  table_global.SetField(WELLPATH_STR_NAME, m_pWellPath->GetWellName().toAscii().constData());
  table_global.SetField(WELLPATH_STR_TYPE_NAME, m_pWellPath->GetWellType().toAscii().constData());
  table_global.SetField(WELLPATH_STR_FIELD_NAME, m_pWellPath->GetFieldName().toAscii().constData());
  table_global.SetField(WELLPATH_STR_LOC_NAME, m_pWellPath->GetLocationName().toAscii().constData());
  table_global.SetField(WELLPATH_STR_COUNTRY, m_pWellPath->GetCountry().toAscii().constData());
  table_global.SetField(WELLPATH_STR_CUST_NAME, m_pWellPath->GetCustomerName().toAscii().constData());
  table_global.SetField(WELLPATH_STR_CUST_CONT, m_pWellPath->GetCustomerContact().toAscii().constData());
  table_global.SetField(WELLPATH_STR_JOB_NO, m_pWellPath->GetJobNumber().toAscii().constData());
  table_global.SetField(WELLPATH_SPUDDATE_DAY, (long)m_pWellPath->GetSpudDate().day());
  table_global.SetField(WELLPATH_SPUDDATE_MONTH, (long)m_pWellPath->GetSpudDate().month());
  table_global.SetField(WELLPATH_SPUDDATE_YEAR, (long)m_pWellPath->GetSpudDate().year());

  table_global.SetField(WELLPATH_GLB_SEALEVEL, m_pWellPath->MeanSeaLevel());
  table_global.SetField(WELLPATH_GLB_GROUND_OR_SEABED_LEVEL, m_pWellPath->MeanGroundOrSeabedLevel());

  if (m_pWellPath->DefPointList().First()) {
    table_global.SetField(WELLPATH_GLB_START_NORTHING, m_pWellPath->DefPointList().First()->Northing());
    table_global.SetField(WELLPATH_GLB_START_EASTING, m_pWellPath->DefPointList().First()->Easting());
    table_global.SetField(WELLPATH_GLB_START_DEPTH, m_pWellPath->DefPointList().First()->TVD());
  }
  table_global.RS().Update();
  table_global.Close();
  database.Close();

  return true;
}

#endif // _WIN64
