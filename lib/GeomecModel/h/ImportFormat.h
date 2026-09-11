// ImportFormat.h: interface for the CImportFormat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMPORTFORMAT_H__8D15F5ED_095D_482E_B384_4CA222FE5E64__INCLUDED_)
#define AFX_IMPORTFORMAT_H__8D15F5ED_095D_482E_B384_4CA222FE5E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AnalysisType.h"
#include "StringNoCaseLess.h"
#include "ivaluecomponent.h"

class CValueType;
class CModelBase;
class IValueSet;
class IPointSet;
class CPointSet;
class IElementSet;
class CNewWellPathInput;

class CGeomecTime;
class CStringNoCaseLess;

namespace geo {
  class CValue;
  class IElement;
  class CPoint;
}

#include <map>
#include <vector>

#include <QVector>

#include "SingleQuantity.h"

// The IImportFormat class parses every line by the 
class IImportFormat
{
  // this map contains aliases for property names that are imported.
  // so for instance, if you want to have a 'PROPERTY Some_Name' in the header of the 
  // file that is imported, but it is really a 'Stress' tensor (a name the valuetypefactory knows)
  // make sure that 'Some_Name' is mapped to 'Stress' via this map. It is used in IImportFormat::CreateValueType.
  std::map<std::string, std::string> m_ValueTypeAliases;

  std::map<std::string, CGeomecTime> m_mpDepTimes;
public:
  typedef std::pair<int, int> TValueType;
  typedef IValueDomainScalar::TValueVec TValueVec;
private:
  CValueType* m_pValueType;
  int m_nNextComponent;
  CModelBase& m_model;
protected:
  IImportFormat(CModelBase& model);	// Default constructor creates the map
  virtual IValueSet* OnCreateValueSet(const QString& strName) = 0;
  std::vector<int> m_vcValueSet;
  typedef std::map<QString, IValueComponentBase*, CStringNoCaseLess> TComponentMap;
  TComponentMap m_mpComponent;
  void CreateAliases();
public:
  typedef std::vector<double> TDoubleVec;
  virtual void AppendColumn(const QString& sName);	// Appends a column to the format
  virtual void ValidateColumns() = 0;					// Validates the header
  virtual void ValidateRows() = 0;					// Validates the numbers
  virtual int ColumnSize() const = 0;
  virtual void AppendRow(const TValueVec &vcRow) = 0;
  virtual IPointSet& PointSet() = 0;
  const CModelBase& Model() const;
  CModelBase &Model() { return m_model; }
  void AddTime(std::string str, int year, int month);
  inline const std::map<std::string, CGeomecTime> &Times() const { return m_mpDepTimes; }
  typedef std::vector<QString> TStringVec;
  CValueType* CreateValueType( const TStringVec& vcType );
};

// CResultFormat is the format for importing results for the current mesh
class CResultImportFormat : public IImportFormat
{
public:
  CResultImportFormat(CModelBase &model);
  virtual ~CResultImportFormat();
  virtual int ColumnSize() const;
  virtual void AppendRow(const TValueVec &vcRow);
  virtual IPointSet& PointSet();
  virtual void AppendColumn(const QString& sName);

  void Unit(CQuantity::UNIT unit) { m_Unit = unit; }
  CQuantity::UNIT Unit() const { return m_Unit; }	
  
  const geo::IElement& Element() const;
  virtual void ValidateColumns();
  virtual void ValidateRows();	
protected:
  virtual IValueSet* OnCreateValueSet(const QString& strName);
private:
  int ResultTypeIndex(const QString& typeName) const;
  class CResultDescription
  {
  private:
    friend class CResultImportFormat;
  // constructor for 'ignore'
  CResultDescription(CResultImportFormat& parent);
    CResultDescription(CResultImportFormat& parent, 
                     int ResultType,
               bool bFault,
               int nDepStage, 
               const CAnalysisType& antype);

    int				      m_nResultType;
    CResultImportFormat&  m_parent;
    int				      m_nDepStage;
    CAnalysisType		m_antype;
    bool			      m_bFault;
  bool m_bIgnore;

  public:
    bool AddResultValue(int nLocalNodeIndex, 
              const geo::CValue &value);
    bool IsStress() const;
    bool IsPressure() const;
  bool IsPreconPressure() const;
    bool IsStrain() const;
    bool IsFault() const;
    bool IsFaultElement() const;
    double FieldFactor() const;
  };

  void ProcessResult(const QString &sResult, int nColumn);
  int ExtractDepletionStage(const QString &sResult) const;
  
  int m_nElementNodeColumn;	// used to keep track if we read the nodenr and elementnr column
  CQuantity::UNIT m_Unit;
  std::vector<CResultDescription*> m_vcResultDescriptions;
  std::vector<bool>                m_vcElementRead;
  int								 m_nCurrentNode;
  const geo::IElement				*m_pCurrentElement;
};

class CNodalImportFormat : public IImportFormat
{
public:
  typedef enum AXIS_SYSTEM { UNKNOWN, AXIS3D, AXIS2D } TAxisSystem;
private:
  enum COORDINATE_TYPE { CT_NORTHING, CT_EASTING, CT_T, CT_N, CT_DEPTH };
  typedef std::map<int, COORDINATE_TYPE> TCoordinateMap;
  TCoordinateMap m_mpCoordinate;
  typedef std::map<int, int> TPointMap;
  TPointMap m_mpPoint;
  int m_nNodeIndex;
  IElementSet* m_pElementSet;
  CPointSet*   m_pPointSet;
  void SetCoordinate(COORDINATE_TYPE coordinate_type);
  bool IsCoordinate(COORDINATE_TYPE coordinate_type) const;

protected:
  virtual IValueSet* OnCreateValueSet(const QString& strName);
  public:
  // Contruction
  CNodalImportFormat(IElementSet& element_set);
  CNodalImportFormat(CPointSet& point_set);
  // Access to element set if any ...
  IElementSet& ElementSet();
  const IElementSet& ElementSet() const;

  // axis system
  TAxisSystem AxisSystem() const;

  // Insert and validation
  virtual void AppendColumn(const QString& sName);	// Appends a column to the format
  virtual void ValidateColumns();
  virtual void ValidateRows();

  virtual int ColumnSize() const;
  virtual void AppendRow(const TValueVec &vcRow);
  virtual IPointSet& PointSet();	
  int MeshNodeIndex(int nFileIndex) const;
};

/*!
  Format to create elements sets
*/
class CElementImportFormat : public IImportFormat
{
  CNodalImportFormat& m_nodal_format;
  int m_nNodeIndex;
  int m_nElementIndex;
  int m_nNodeNr;
  std::vector<TValueVec> m_vcElementData;
protected:
  virtual IValueSet* OnCreateValueSet(const QString& strName);
public:
  CElementImportFormat(CNodalImportFormat& nodal_format);

  // Insert and validation
  virtual void AppendColumn(const QString& sName);	// Appends a column to the format
  virtual void ValidateColumns();
  virtual void ValidateRows();
  virtual int ColumnSize() const;
  virtual void AppendRow(const TValueVec &vcRow);
  virtual IPointSet& PointSet();
  void SetElementType(const QString& sElementType);
};

/*!
  Format to import wells
*/
class CWellPathInputImportFormat : public IImportFormat
{
  typedef enum
  {
    UNIT_SI,
    UNIT_FIELD
  } TUnit;

  TUnit m_Unit;

  std::vector <int> m_column;
  QVector<geo::CPoint> m_vcPoint;
  CNewWellPathInput*       m_pNewWellPathInput;
  int m_lastColumnIndex;
  void setColumn(int nColumn, const QString &sName);
protected:
  virtual IValueSet* OnCreateValueSet(const QString& strName);
public:
  CWellPathInputImportFormat(CNewWellPathInput *pNewWellPathInput);

  bool OnUnit(const QString& sUnit);
  
  // Insert and validation
  virtual void AppendColumn(const QString& sName);	// Appends a column to the format
  virtual void ValidateColumns();
  virtual void ValidateRows();
  virtual int ColumnSize() const;
  virtual void AppendRow(const TValueVec &vcRow);
  virtual IPointSet& PointSet();

  virtual CNewWellPathInput* NewWellPathInput() // wjrx mantis 3185
  { return m_pNewWellPathInput; }
  virtual const CNewWellPathInput* NewWellPathInput() const
  { return m_pNewWellPathInput; }
};

#endif // !defined(AFX_IMPORTFORMAT_H__8D15F5ED_095D_482E_B384_4CA222FE5E64__INCLUDED_)
