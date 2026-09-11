// DexWellInterface.cpp: implementation of the CDexWellInterface class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "wellschemeutils.h"

#include "wellpathbase.h"
#include "DexWellInterface.h"
#include "DexDepthTypeDlg.h"

//#ifdef _DEBUG
//	#pragma comment(linker, "/DELAYLOAD:dexuid.dll")
//	#pragma comment(linker, "/DELAYLOAD:dexd.dll")
//#else
//	#pragma comment(linker, "/DELAYLOAD:dexui.dll")
//	#pragma comment(linker, "/DELAYLOAD:dex.dll")
//#endif
//
//

#ifdef USE_DEX
#include "dexui.h"
#include "dexiface.h"
#endif



#include <qdatetime.h>
#include <qstringlist.h>

#define EXPORT_OBJECTS 3
static char *EXPORT_NAMES[] =
{
  "General Well Information",
  "Detail Well Information",
  "Wellpath Plans"
};

#define IMPORT_OBJECTS 1
static char *IMPORT_NAMES[] =
{
  "General Well Information",
  "Detail Well Information"

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CMdAziInc
{
public:
  double m_tmd, m_azi, m_inc;
  CMdAziInc(double tmd, double azi, double inc) { m_tmd = tmd; m_azi = azi; m_inc = inc; }

  bool operator < (const CMdAziInc &rhs) const { return m_tmd < rhs.m_tmd; }
};

CDexWellInterface::CDexWellInterface(well::CWellPathBase &well_path, QString app_name, QString version): m_WellPath(well_path), m_app_name(app_name), m_version(version)
{

}

CDexWellInterface::CDexWellInterface(well::CWellPathBase &well_path): m_WellPath(well_path)
{
  // Construct
}

CDexWellInterface::~CDexWellInterface()
{

}

BOOL CDexWellInterface::ImportFormationSections(QString path_name, QString &strError, QList<double> &tmd_values, QStringList &names)
{
#ifdef USE_DEX

  if(!InitDEXForImport(path_name, strError))
    return FALSE;

  CDexDepthTypeDlg::DEPTH_TYPE type;
  CDexDepthTypeDlg dlg;
  if(dlg.DoModal() == IDOK) // there is no other way out, only one button :)
    type = dlg.DepthType();

  CWaitCursor cursor;
  char *szBusinessObject;
  char *szAuthor;
  char *szVersion;
  char *szWellID;
  char *szDate;
  char *szTable;
  char *szField;
  char *szType;
  char *szUnit;
  char *szParent;
  char *szValue;
  int iRecNo;
  CLengthQuantity length;
//	QValueList<double> prognosed;
//	QValueList<double> logged;

  //bool tmds_cleared = false;
  tmd_values.clear();	
  while(DexGetNextObject(&szBusinessObject, &szAuthor, &szVersion, &szWellID, &szDate))
  {
    BusinessObj(szBusinessObject);
    Author(szAuthor);
    Version(szVersion);
    WellID(szWellID);
    Date(szDate);

    if(BusinessObj() == "Formation Tops")
    {	
      while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
      {
        Table(szTable);
        Field(szField);
        Type(szType);
        Unit(szUnit);
        Value(szValue);
        Parent(szParent);

        if(Table() == "FORMATION-TOPS")
        {
          if(Field() == "FORMATION-NAME")
          {
            names.push_back(Value());
            //tmd_values.push_back(-1.0); // push a dummy value
          }
          else if(Field() == "PROGNOSED-DEPTH" && type == CDexDepthTypeDlg::PROGNOSED)
          { // this will be available first, but ideally we want the Logged Depth...
            tmd_values.push_back(length.Convert(Value().toDouble(), IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT));
          }
          else if(Field() == "LOGGED-DEPTH" && type == CDexDepthTypeDlg::LOGGED)
          { // Ah! Logged depth is available! Forget the Prognosed Depth, this is the real deal!!
            tmd_values.push_back(length.Convert(Value().toDouble(), IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT));
          }
        }
      }
    }
  }

  return CloseDEX(strError);

#else // USE_DEX

  return FALSE;

#endif // USE_DEX
}

BOOL CDexWellInterface::ImportWellPathFromDEX(QString path_name, QString &strError)
{
#ifdef USE_DEX

  if(!InitDEXForImport(path_name, strError))
    return FALSE;

  char *szBusinessObject;
  char *szAuthor;
  char *szVersion;
  char *szWellID;
  char *szDate;
  bool bWellGeometrySet = false;

  while(DexGetNextObject(&szBusinessObject, &szAuthor, &szVersion, &szWellID, &szDate))
  {
    BusinessObj(szBusinessObject);
    Author(szAuthor);
    Version(szVersion);
    WellID(szWellID);
    Date(szDate);

    if(BusinessObj() == "General Well Information")
      ImportGeneralWellInfo();

    if(BusinessObj() == "Detail Well Information")
      ImportDetailWellInfo();

    // NB: order of Survey, Non-Definitive Surveys and Wellpath plans are of importance here.....
    // Setting the wellpath geometry in order most reliable data
    if(BusinessObj() == "Survey" && !bWellGeometrySet)
    { // survey available: Great! use it...
      ImportWellGeometry(SURVEY);
      bWellGeometrySet = true;
    }
    if(BusinessObj() == "Non-Definitive Surveys" && !bWellGeometrySet)
    { // the non-definitive survey available, use this one...
      ImportWellGeometry(SURVEY);
      bWellGeometrySet = true;
    }
    if(BusinessObj() == "Wellpath Plans" && !bWellGeometrySet)
    { // no surveys available? use the well plans...
      ImportWellGeometry(PLAN);
      bWellGeometrySet = true;
    }
  }
  
  SetWellPathGeometry();
  return CloseDEX(strError);

#else // USE_DEX

  return FALSE;

#endif
}

BOOL CDexWellInterface::ExportToDEX(QString path_name, QString file_name, QString &strError) const
{
#ifdef USE_DEX

    QString sPath = path_name.left(path_name.length() - file_name.length());

    // szChosen is list of available objects to export - passed to dex ui to determine
    // which of the available objects to export
    char *szChosen [EXPORT_OBJECTS];
    int iTotal;
    for(iTotal = 0; iTotal < EXPORT_OBJECTS; iTotal++ )
    {
      szChosen[iTotal] = new char [256];
      strcpy(szChosen[iTotal], EXPORT_NAMES[iTotal]);
    }

    CStringList slstChosen;

    CFileStatus status;
    // determine which business objects to export
    // slstChosen filled in with list
    if(!CFile::GetStatus(path_name.toAscii().constData(), status) || DexImpExpDialog( FALSE, sPath.toAscii().constData(), file_name.toAscii().constData(), AppName().toAscii().constData(), m_WellPath.GetWellName().toAscii().constData(), szChosen, &iTotal ) )
    {
      CString sChose;
      for( int i = 0; i < iTotal; i++ )
      {
        slstChosen.AddTail( szChosen[i] );
      }

      char *error_msg;
      BOOL bState;
      bState = DexAttach(&error_msg);
      if(CFile::GetStatus(path_name.toAscii().constData(), status) && !DexOpen(path_name.toAscii().constData(), &error_msg, TRUE))
        bState = false;

      if(bState)
      {
        QString sBusinessObject;
        while( slstChosen.GetCount() > 0 )
        {
          sBusinessObject = slstChosen.RemoveTail();
          if(sBusinessObject == "General Well Information")
            ExportGeneralWellInformation();
          if(sBusinessObject == "Detail Well Information")
            ExportDetailWellInformation();
          if(sBusinessObject == "Wellpath Plans")
            ExportWellPlans();
        }

        bState = DexSave(path_name.toAscii().constData(), &error_msg);
      }

      if(!bState)
        strError = error_msg; // deep copy
      
      DexDetach();
      DexBrowseDialog( sPath.toAscii().constData(), file_name.toAscii().constData() );
       
      return bState;
    }

    strError = QString("Unable to export wellpath.");

#endif // USE_DEX

    return FALSE;
}

void CDexWellInterface::ExportGeneralWellInformation() const
{
#ifdef USE_DEX

  // Create the "General Well Information" businessobject
  if(!DexCreateObject("General Well Information", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData()))
    DexPutObject("General Well Information", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData());

  // Put values in the "General Well Information" businessobject
  if(m_WellPath.GetDescription().toAscii().constData())
    ExportValue("GENERAL", "WELL-DESCRIPTION", m_WellPath.GetDescription().toAscii().constData());
  else
    ExportValue("GENERAL", "WELL-DESCRIPTION", "NO WELL DESCRIPTION AVAILABLE");

  switch(m_WellPath.GetLocation())
  {
  case well::CWellPathBase::ONSHORE:
    ExportValue("GENERAL", "LOCATION", "ONSHORE");
    break;
  case well::CWellPathBase::PLATFORM:
    {
      CLengthQuantity length;
      ExportValue("GENERAL", "LOCATION", "PLATFORM");
      ExportValue("GENERAL", "WATER-DEPTH", length.Convert(m_WellPath.HeightOfWaterColumn(), IQuantityDouble::FIELD_UNIT, IQuantityDouble::SI_UNIT));
      break;
    }
  case well::CWellPathBase::SUBSEA:
    {
      CLengthQuantity length;
      ExportValue("GENERAL", "LOCATION", "SUBSEA");
      ExportValue("GENERAL", "WATER-DEPTH", length.Convert(m_WellPath.HeightOfWaterColumn(), IQuantityDouble::FIELD_UNIT, IQuantityDouble::SI_UNIT));
      break;
    }
  default:
    assert(false);
  }
  ExportValue("GENERAL", "REF-POINT", "RKB");
  ExportValue("GENERAL", "ELEVATION", m_WellPath.MeanGroundOrSeabedLevel().Value(IQuantityDouble::FIELD_UNIT));
  
  
  // ***************************************************** 

#endif // USE_DEX
}

void CDexWellInterface::ExportDetailWellInformation() const
{
#ifdef USE_DEX

  // Create the "Detail Well Information" businessobject
  if(!DexCreateObject("Detail Well Information", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData()))
    DexPutObject("Detail Well Information", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData());

  // Put values in the "Detail Well Information" businessobject
  if(m_WellPath.GetWellName().toAscii().constData())
    ExportValue("DETAILS", "COMMON-WELL-NAME", m_WellPath.GetWellName().toAscii().constData());
  else
    ExportValue("DETAILS", "COMMON-WELL-NAME", "NO WELL NAME AVAILABLE");

  if(m_WellPath.GetWellType().toAscii().constData())
    ExportValue("DETAILS", "WELL-TYPE", m_WellPath.GetWellType().toAscii().constData());
  else
    ExportValue("DETAILS", "WELL-TYPE", "NO WELL TYPE AVAILABLE");

  if(m_WellPath.GetFieldName().toAscii().constData())
    ExportValue("DETAILS", "FIELD", m_WellPath.GetFieldName().toAscii().constData());
  else
    ExportValue("DETAILS", "FIELD", "NO FIELD NAME AVAILABLE");

  if(m_WellPath.GetLocationName().toAscii().constData())
    ExportValue("DETAILS", "LOCATION", m_WellPath.GetLocationName().toAscii().constData());
  else
    ExportValue("DETAILS", "LOCATION", "NO LOCATION NAME AVAILABLE");

  if(m_WellPath.GetCountry().toAscii().constData())
    ExportValue("DETAILS", "COUNTRY", m_WellPath.GetCountry().toAscii().constData());
  else
    ExportValue("DETAILS", "COUNTRY", "NO COUNTRY NAME AVAILABLE");

  ExportValue("DETAILS", "SPUD-DATE", m_WellPath.GetSpudDate());
  // *****************************************************

#endif // USE_DEX
}

void CDexWellInterface::ExportWellPlans() const
{
#ifdef USE_DEX

  // Create the "Wellpath Plans" businessobject
  if(!DexCreateObject("Wellpath Plans", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData()))
    DexPutObject("Wellpath Plans", AppName().toAscii().constData(), Version().toAscii().constData(), WellRepresentation().toAscii().constData());

  // Put values in the "Wellpath Plans" businessobject
  well::CWellDefinitionPointList &pt_list = m_WellPath.DefPointList();
  well::CWellDefinitionPointList::Iterator it = pt_list.begin();
  well::CWellPoint *point = 0;
  well::CWellPoint *temp;
  double tmd;

  if(it != pt_list.end())
    point = *it;

  ExportValue("PLAN", "COMMENT", ("Written by " + AppName()).toAscii().constData());
  ExportValue("PLAN", "DATE", QDate::currentDate());
  CCurvatureQuantity curv;
  while(point)
  {
    ExportValue("SECTION", "MD", point->TMD().Value(IQuantityDouble::FIELD_UNIT));
    ExportValue("SECTION", "TVD", point->TVD().Value(IQuantityDouble::FIELD_UNIT));
    ExportValue("SECTION", "INCLINATION", point->Inclination().Value());
    ExportValue("SECTION", "AZIMUTH", point->Azimuth().Value());
    ExportValue("SECTION", "NORTH", point->Northing().Value(IQuantityDouble::FIELD_UNIT));
    ExportValue("SECTION", "EAST", point->Easting().Value(IQuantityDouble::FIELD_UNIT));

    if(point == pt_list.First())
      ExportValue("SECTION", "DLS", 0.0);
    else
    {
      tmd = temp->TMD().Value() + ( (point->TMD().Value() - temp->TMD().Value()) / 2.0 );
      curv.Value(m_WellPath.GetDogLegSeverity(tmd), IQuantityDouble::FIELD_UNIT);
      ExportValue("SECTION", "DLS", curv.Value());
    }

    temp = point;
    ++it;
  if(it != pt_list.end())
      point = *it;
  else
      point = 0;
  }

#endif // USE_DEX
}

void CDexWellInterface::ImportGeneralWellInfo()
{
#ifdef USE_DEX

//	QString sField;
//	QString sTable;
//	QString sValue;
//	QString sType;
//	QString sUnit;
//	QString sParent;
  QString sRefPoint;


  char *szField;
  char *szTable;
  char *szValue;
  char *szUnit;
  char *szParent;
  char *szType;
  int iRecNo;
  int iOldRecord = -1;
  

  while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
  {
    Table(szTable);
    Field(szField);
    Type(szType);
    Unit(szUnit);
    Value(szValue);
    Parent(szParent);

    if(Table() == "GENERAL")
    {
      if(Field() == "WELL-DESCRIPTION")
      {
        m_WellPath.SetDescription(Value());
      }
      else if(Field() == "LOCATION")
      {
        if(Value() == "ONSHORE")
          m_WellPath.SetLocation(well::CWellPathBase::ONSHORE);
        if(Value() == "PLATFORM")
          m_WellPath.SetLocation(well::CWellPathBase::PLATFORM);
        if(Value() == "SUBSEA")
          m_WellPath.SetLocation(well::CWellPathBase::SUBSEA);
      }
      else if(Field() == "REF-POINT")
        sRefPoint = Value();
      else if(Field() == "ELEVATION")
      {	
        switch(m_WellPath.GetLocation())
        {
        case well::CWellPathBase::ONSHORE:
          {
            if(sRefPoint == "RKB")
              m_WellPath.MeanGroundOrSeabedLevel().Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
            else
              m_WellPath.MeanGroundOrSeabedLevel().SetValue(0.0);
            break;
          }
        case well::CWellPathBase::PLATFORM:
        case well::CWellPathBase::SUBSEA:
          {
            if(sRefPoint == "RKB")
              m_WellPath.MeanSeaLevel().Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
            else
              m_WellPath.MeanSeaLevel().SetValue(0.0);
            break;
          }
        default:
          assert(false);
        }
      }
      else if(Field() == "WATER-DEPTH")
      {
        double water_depth = Value().toDouble();
        switch(m_WellPath.GetLocation())
        {
        case well::CWellPathBase::ONSHORE:
          break;
        case well::CWellPathBase::PLATFORM:
        case well::CWellPathBase::SUBSEA:
          {	
            m_WellPath.MeanGroundOrSeabedLevel().Value(water_depth + m_WellPath.MeanSeaLevel().Value(IQuantityDouble::FIELD_UNIT), IQuantityDouble::FIELD_UNIT);
            break;
          }
        default:
          assert(false);
        }
      }
    }
  }

#endif // USE_DEX

}

void CDexWellInterface::ImportDetailWellInfo()
{
#ifdef USE_DEX

//	QString sField;
//	QString sTable;
//	QString sValue;
//	QString sType;
//	QString sUnit;
//	QString sParent;
//	QString sRefPoint;


  char *szField;
  char *szTable;
  char *szValue;
  char *szUnit;
  char *szParent;
  char *szType;
  int iRecNo;
  int iOldRecord = -1;
  

  while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
  {
    Table(szTable);
    Field(szField);
    Type(szType);
    Unit(szUnit);
    Value(szValue);
    Parent(szParent);

    if(Table() == "DETAILS")
    {
      if(Field() == "COMMON-WELL-NAME")
        m_WellPath.SetWellName(Value());
      else if(Field() == "WELL-TYPE")
        m_WellPath.SetWellType(Value());
      else if(Field() == "FIELD")
        m_WellPath.SetFieldName(Value());
      else if(Field() == "LOCATION")
        m_WellPath.SetLocationName(Value());
      else if(Field() == "COUNTRY")
        m_WellPath.SetCountry(Value());
      else if(Field() == "SPUD-DATE")
      {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        DexStringToDate(Value().toAscii().constData(), &year, &month, &day, &hour, &minute);
        m_WellPath.SetSpudDate(QDate(year, month, day));
      }
    }
  }

#endif // USE_DEX
}

void CDexWellInterface::ImportWellGeometry(WP_GEOMETRY_INPUT_TYPE geom_type)
{
#ifdef USE_DEX

  char *szField;
  char *szTable;
  char *szValue;
  char *szUnit;
  char *szParent;
  char *szType;
  int iRecNo;
  //int iOldRecord = -1;
  bool bFirstFound = false;
  CLengthQuantity length;
  int size;

  while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
  {
    Table(szTable);
    Field(szField);
    Type(szType);
    Unit(szUnit);
    Value(szValue);
    Parent(szParent);

    switch(geom_type)
    {
    case SURVEY:
      {
        if(Parent() == "SURVEY")
        {
          if(Table() == "STATION")
          {
            if(Field() == "TVD")
            {
              m_vc_tvd.resize(m_vc_tvd.size() + 1);
              length.Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
              size = m_vc_tvd.size() - 1;
              m_vc_tvd[size] = length.Value();
            }
            else if(Field() == "MD")
            {
              m_vc_tmd.resize(m_vc_tmd.size() + 1);
              length.Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
              size = m_vc_tmd.size() - 1;
              m_vc_tmd[size] = length.Value();
            }
            else if(Field() == "INCLINATION")
            {
              m_vc_inc.resize(m_vc_inc.size() + 1);
              size = m_vc_inc.size() - 1;
              m_vc_inc[size] = Value().toDouble();
            }
            else if(Field() == "AZIMUTH")
            {
              m_vc_azi.resize(m_vc_azi.size() + 1);
              size = m_vc_azi.size() - 1;
              m_vc_azi[size] = Value().toDouble();
            }
          }
        }
        break;
      }
    case PLAN:
      {
        if(Parent() == "PLAN")
        {
          if(Table() == "SECTION")
          {
            if(Field() == "TVD")
            {
              m_vc_tvd.resize(m_vc_tvd.size() + 1);
              length.Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
              size = m_vc_tvd.size() - 1;
              m_vc_tvd[size] = length.Value();
            }
            else if(Field() == "MD")
            {
              m_vc_tmd.resize(m_vc_tmd.size() + 1);
              length.Value(Value().toDouble(), IQuantityDouble::FIELD_UNIT);
              size = m_vc_tmd.size() - 1;
              m_vc_tmd[size] = length.Value();
            }
            else if(Field() == "INCLINATION")
            {
              m_vc_inc.resize(m_vc_inc.size() + 1);
              size = m_vc_inc.size() - 1;
              m_vc_inc[size] = Value().toDouble();
            }
            else if(Field() == "AZIMUTH")
            {
              m_vc_azi.resize(m_vc_azi.size() + 1);
              size = m_vc_azi.size() - 1;
              m_vc_azi[size] = Value().toDouble();
            }
          }
        }
        break;
      }
    default:
      assert(false);
    }
  }

#endif // USE_DEX
}

void CDexWellInterface::SetWellPathGeometry()
{
  if(m_vc_tmd.size() == 0)
    return;
//	QMemArray<double> temp_depth;
  int nStartIndex = 0;
  double dLevel;

  switch(m_WellPath.GetLocation())
  {
  case well::CWellPathBase::ONSHORE:
    {
      dLevel = m_WellPath.MeanGroundOrSeabedLevel().Value(IQuantityDouble::FIELD_UNIT);
      break;
    }
  case well::CWellPathBase::PLATFORM:
  case well::CWellPathBase::SUBSEA:
    {
      dLevel = m_WellPath.MeanSeaLevel().Value(IQuantityDouble::FIELD_UNIT);
      break;
    }
  default:
    assert(false);
  }

  std::set<CMdAziInc> set_in_right_order;
  int i, j, k, size;

  assert(m_vc_tmd.size() == m_vc_inc.size());
  assert(m_vc_tmd.size() == m_vc_azi.size());
  for(size_t t = 0; t < m_vc_tmd.size(); t++)
  {
    set_in_right_order.insert(CMdAziInc(m_vc_tmd[t], m_vc_azi[t], m_vc_inc[t]));
  }

  size = 0;
  for(std::set<CMdAziInc>::iterator it = set_in_right_order.begin(); it != set_in_right_order.end(); it++)
  {
    m_vc_tmd[size] = it->m_tmd;
    m_vc_azi[size] = it->m_azi;
    m_vc_inc[size] = it->m_inc;
    size++;
  }
  
  if(m_vc_tvd.size() == 0)
  { // must be a vertical wellpath, use the tmd
  //	temp_depth = m_vc_tmd;
    for(i = 0; i < m_vc_tmd.size(); i++)
    {
      if(m_vc_tmd[i] >= dLevel)
      {
        nStartIndex = i;
        break;
      }
    }
  }
  else
  {
    assert(m_vc_tmd.size() == m_vc_tvd.size());
    for(i = 0; i < m_vc_tvd.size(); i++)
    {
      if(m_vc_tmd[i] >= dLevel && m_vc_tvd[i] >= dLevel)
      {
        nStartIndex = i;
        break;
      }
    }
  }

  if(nStartIndex == 0)
  {
    m_WellPath.InitFromArray_tmd_azi_inc(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, m_vc_tmd, m_vc_azi, m_vc_inc);
    return;
  }


  QVector<double> tmd;
  QVector<double> azi;
  QVector<double> inc;
  tmd.resize(m_vc_tmd.size() - nStartIndex);
  azi.resize(m_vc_tmd.size() - nStartIndex);
  inc.resize(m_vc_tmd.size() - nStartIndex);
  
  for(k = 0, j = nStartIndex; j < m_vc_tmd.size(); j++, k++)
  {
    tmd[k] = m_vc_tmd[j];
    azi[k] = m_vc_azi[j];
    inc[k] = m_vc_inc[j];
  }

  m_WellPath.InitFromArray_tmd_azi_inc(0.0, 0.0, 0.0, 0.0, 0.0, m_vc_tmd[nStartIndex], tmd, azi, inc);
  
}


QString CDexWellInterface::BrowseForDEXFile()
{
#ifdef USE_DEX

  DexBrowseDialog( NULL, NULL );

#endif // USE_DEX

  return "";
}

QString CDexWellInterface::WellRepresentation() const
{
  QString  str_ret;
//	if(m_WellPath.GetLocationName().toAscii().constData())
//	{
//		str_ret += m_WellPath.GetLocationName();
//		str_ret += " ";
//	}
//	if(m_WellPath.GetFieldName().toAscii().constData())
//	{
//		str_ret += m_WellPath.GetFieldName();
//		str_ret += " ";
//	}

  if(!m_WellPath.GetWellName().isEmpty())
    str_ret += m_WellPath.GetWellName();

  if(str_ret.isEmpty())
    return QString("UNKNOWN");
  
  return str_ret;
}


