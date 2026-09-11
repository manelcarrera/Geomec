// DexCasingMatInterface.cpp: implementation of the CDexCasingMatInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wellschemeutils.h"
#include "DexCasingMatInterface.h"
#include <qstringlist.h>

#ifdef USE_DEX
#include "dexui.h"
#include "dexiface.h"
#endif

//#ifdef _DEBUG
//	#pragma comment(linker, "/DELAYLOAD:dexuid.dll")
//	#pragma comment(linker, "/DELAYLOAD:dexd.dll")
//#else
//	#pragma comment(linker, "/DELAYLOAD:dexui.dll")
//	#pragma comment(linker, "/DELAYLOAD:dex.dll")
//#endif



#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif  // _MSC_VER
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDexCasingMatInterface::CDexCasingMatInterface()
{

}

CDexCasingMatInterface::~CDexCasingMatInterface()
{

}

BOOL CDexCasingMatInterface::ImportCasingDefinition(QString path_name, QString &strError, QList<SCasingDefinition*> &casing_definitions)
{
#ifdef USE_DEX

  if(!InitDEXForImport(path_name, strError))
    return FALSE;

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
  int iOldRec = -1;

  SCasingDefinition *definition = 0;

  QStringList grade_names;
  CSinglePressure qPres;
  CLengthQuantity qLength;
  CWeightPerUnitLengthQuantity qWeight;

  m_GradesMap.clear();

  while(DexGetNextObject(&szBusinessObject, &szAuthor, &szVersion, &szWellID, &szDate))
  {
    BusinessObj(szBusinessObject);
    Author(szAuthor);
    Version(szVersion);
    WellID(szWellID);
    Date(szDate);

    if(BusinessObj() == "Casing Scheme")
    {
      while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
      {
        Table(szTable);
        Field(szField);
        Type(szType);
        Unit(szUnit);
        Value(szValue);
        Parent(szParent);

        if(Table() == "PIPES-IN-USE")
        {
          if(iRecNo != iOldRec)
          {
            definition = new SCasingDefinition;
            casing_definitions.insert(casing_definitions.count(), definition);
            iOldRec = iRecNo;
          }

          if(Field() == "OD")
          {
            casing_definitions.at(iRecNo)->outer_diameter = Value().toDouble(); // inch
            casing_definitions.at(iRecNo)->outer_diameter = casing_definitions.at(iRecNo)->outer_diameter * (1.0/12.0); // ft
            casing_definitions.at(iRecNo)->outer_diameter = qLength.Convert(casing_definitions.at(iRecNo)->outer_diameter, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT); // m
          }
          else if(Field() == "GRADE")
          {
            grade_names.push_back(Value());
          }
          else if(Field() == "ID")
          {
            casing_definitions.at(iRecNo)->inner_diameter = Value().toDouble();
            casing_definitions.at(iRecNo)->inner_diameter = casing_definitions.at(iRecNo)->inner_diameter * (1.0/12.0); // ft
            casing_definitions.at(iRecNo)->inner_diameter = qLength.Convert(casing_definitions.at(iRecNo)->inner_diameter, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT); // m
          }
          else if(Field() == "WEIGHT")
          {
            casing_definitions.at(iRecNo)->weight = Value().toDouble();
            casing_definitions.at(iRecNo)->weight = qWeight.Convert(casing_definitions.at(iRecNo)->weight, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT);
          }
          else if(Field() == "YIELD-STRENGTH")
          {	
            casing_definitions.at(iRecNo)->yield_strength = Value().toDouble();
            casing_definitions.at(iRecNo)->yield_strength = qPres.Convert(casing_definitions.at(iRecNo)->yield_strength, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT);
          }
        }
      }
    }
  }
  assert(casing_definitions.count() == grade_names.count());

  // set the names
  QMap<QString, QString> temp_map;
  QMap<QString, QString>::Iterator iter;
  QString name, temp;
  int nr = 0;
  for(int l = 0; l < grade_names.count(); l++)
  {
    name = grade_names[l] + " " + ConvertNumberToString(qLength.Convert(casing_definitions.at(l)->outer_diameter, IQuantityDouble::FIELD_UNIT, IQuantityDouble::SI_UNIT) * 12.0);
    
    iter = temp_map.find(name);
    
    if(iter == temp_map.end())
    {
      temp_map.insert(name, name);
      casing_definitions.at(l)->casing_type = name;
      continue;
    }

    // this name already exists, we're gonna number 'em!
    temp = name;
    while(iter != temp_map.end())
    {
      name = temp;
      name = name + QString("(%1)").arg(++nr);
      iter = temp_map.find(name);
    }

    casing_definitions.at(l)->casing_type = name;
  }

  ImportCasingGrades();

  if(m_GradesMap.count() == 0)
  {
    // set defaults
    for(int j = 0; j < casing_definitions.count(); j++)
    {	
      casing_definitions.at(j)->young_modulus = 200000; // MPa
      casing_definitions.at(j)->poisson_ratio = 0.3;	  // --
      casing_definitions.at(j)->thermal_exp = 13E-6;	  // 1/K
    //	casing_definitions.at(j)->casing_type = "NO GRADE NAME AVAILABLE";
    }

  }
  else
  {
    QMap<QString, SCasingGrades*>::Iterator it;
    for(int i = 0; i < grade_names.count(); i++)
    {
      it = m_GradesMap.find(grade_names[i]);
      if(it != m_GradesMap.end())
      {
        casing_definitions.at(i)->young_modulus = (*it)->young_modulus;
        casing_definitions.at(i)->poisson_ratio = (*it)->poisson_ratio;
        casing_definitions.at(i)->thermal_exp = (*it)->thermal_exp;
    //		casing_definitions.at(i)->casing_type = grade_names[i] + " " + ConvertNumberToString(casing_definitions.at(i)->outer_diameter);
      }
      
    }
  }

  //clean up
  for(int k = 0; k < grade_names.count(); k++)
  {
    delete m_GradesMap[grade_names[k]];
  }

  return CloseDEX(strError);

#else // USE_DEX

  return FALSE;

#endif // USE_DEX
}

BOOL CDexCasingMatInterface::ImportCasingGrades()
{
#ifdef USE_DEX

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
  int iOldRec = -1;

  CTemperatureQuantity qTemp;
  CSinglePressure qPres;

  m_GradesMap.clear();
  SCasingGrades *grade = 0;
  QString current;
  double thermal;
  bool bGradesPresent = false;

  while(DexGetNextObject(&szBusinessObject, &szAuthor, &szVersion, &szWellID, &szDate))
  {
    BusinessObj(szBusinessObject);
    Author(szAuthor);
    Version(szVersion);
    WellID(szWellID);
    Date(szDate);

    if(BusinessObj() == "Casing Scheme")
    {
      while(DexGetNextValue(&szTable, &szField, &szType, &szUnit, &szValue, &szParent, &iRecNo))
      {
        Table(szTable);
        Field(szField);
        Type(szType);
        Unit(szUnit);
        Value(szValue);
        Parent(szParent);
        
        if(Table() == "GRADES")
        {
          bGradesPresent = true;
          if(iRecNo != iOldRec)
          {
            grade = new SCasingGrades;
            iOldRec = iRecNo;
          }
          else if(Field() == "NAME")
          {
            current = Value();
            m_GradesMap.insert(Value(), grade);
          }
          else if(Field() == "POISSONS-RATIO")
          {
            m_GradesMap[current]->poisson_ratio = Value().toDouble();
          }
          else if(Field() == "EXP-COEF")
          {
            thermal = Value().toDouble(); // 1E-6/F
            thermal = thermal * 1E6;   // 1/F
            thermal = 1.0 / thermal;  // F
            thermal = qTemp.Convert(thermal, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT); // deg Celsius
            thermal = 1.0 / thermal; // 1/degC
            m_GradesMap[current]->thermal_exp = thermal;
          }
          else if(Field() == "YOUNGS-MODULUS")
          {
            m_GradesMap[current]->young_modulus = Value().toDouble();
            m_GradesMap[current]->young_modulus = qPres.Convert(m_GradesMap[current]->young_modulus, IQuantityDouble::SI_UNIT, IQuantityDouble::FIELD_UNIT);
          }
        }

      }
    }
  }

  if(m_GradesMap.count() > 0)
    return TRUE;

#endif // USE_DEX

  return FALSE;

}
