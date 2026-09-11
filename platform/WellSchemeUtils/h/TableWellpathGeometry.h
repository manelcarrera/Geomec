// TableWellpathGeometry.h: interface for the TableWellpathGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLEWELLPATHGEOMETRY_H__113BAFE3_179C_4561_AC79_0FC72B8B46D9__INCLUDED_)
#define AFX_TABLEWELLPATHGEOMETRY_H__113BAFE3_179C_4561_AC79_0FC72B8B46D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN64

#include "DSTable.h"

#define WELLPATH_GEO_TABLE_NAME 					"WellPathGeo"
#define WELLPATH_GEO_TMD							"WellPathGeoTMD"
#define WELLPATH_GEO_AZI							"WellPathGeoAzimuth"
#define WELLPATH_GEO_INC							"WellPathGeoInclination"

#define WELLPATH_GLB_TABLE_NAME 					"WellPathGlobal"
#define WELLPATH_GLB_NORTHING						"WellPathGlobalNorthing"
#define WELLPATH_GLB_EASTING						"WellPathGlobalEasting"
#define WELLPATH_GLB_DEPTH							"WellPathGlobalDepth"
#define WELLPATH_GLB_START_NORTHING					"WellPathStartNorthing"
#define WELLPATH_GLB_START_EASTING					"WellPathStartEasting"
#define WELLPATH_GLB_START_DEPTH					"WellPathStartDepth"

#define WELLPATH_STR_DESCRIPTION					"WellPathDescription"
#define WELLPATH_STR_NAME							"WellPathName"
#define WELLPATH_STR_TYPE_NAME						"WellPathTypeName"
#define WELLPATH_STR_FIELD_NAME						"WellPathFieldName"
#define WELLPATH_STR_LOC_NAME						"WellPathLocationName"
#define WELLPATH_STR_COUNTRY						"WellPathCountryName"
#define WELLPATH_STR_CUST_NAME						"WellPathCustomer"
#define WELLPATH_STR_CUST_CONT						"WellPathCustomenContact"
#define WELLPATH_STR_JOB_NO							"WellPathJobNumber"
#define WELLPATH_SPUDDATE_DAY						"WellPathSpudDateDay"
#define WELLPATH_SPUDDATE_MONTH						"WellPathSpudDateMonth"
#define WELLPATH_SPUDDATE_YEAR						"WellPathSpudDateYear"

#define WELLPATH_GLB_SEALEVEL					    "MeanSeaLevel"
#define WELLPATH_GLB_GROUND_OR_SEABED_LEVEL	        "MeanGroundOrSeabedLevel"

class TableWellpathGeometry : public CDSTable  
{
public:
  virtual ~TableWellpathGeometry();
  TableWellpathGeometry(CDaoDatabase& database);
  virtual void Open();
  virtual void Create();
  virtual CString TableName() const;

};

class TableWellpathGlobal : public CDSTable  
{
public:
  virtual ~TableWellpathGlobal();

  TableWellpathGlobal(CDaoDatabase& database);
  virtual void Open();
  virtual void Create();
  virtual CString TableName() const;

};

#endif // _WIN64

#endif // !defined(AFX_TABLEWELLPATHGEOMETRY_H__113BAFE3_179C_4561_AC79_0FC72B8B46D9__INCLUDED_)

