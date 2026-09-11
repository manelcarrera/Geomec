// PropertyFile.h: interface for the CPropertyFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYFILE_H__438B853D_25C0_462E_AF3F_56AB52ED48CB__INCLUDED_)
#define AFX_PROPERTYFILE_H__438B853D_25C0_462E_AF3F_56AB52ED48CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "FvTextFile.h"
#include "GraphNode.h"

class CElementSet;
class IElementSet;
class IPointSet;
class CPointSet;
class CNodalExportFormat;
class IExportFormat;
class IElementSetExportFormat;
class IImportFormat;
class CResultImportFormat;
class CNodalImportFormat;
class CElementImportFormat;
class CWellPathInputImportFormat;
class IValueComponentBase;
class CModelBase;
class CFemAppModel;

#include "SingleQuantity.h"
#include "IPointSet.h"

class CElementSetFile : public CTextFile
{
private:
  class CHeaderToken : public CTokenTemplate<CElementSetFile>
  {
  public:
  CHeaderToken(CElementSetFile& instance, const QString &sToken, pfToken token);
  virtual bool Read(input_stream& stream, const QString &sToken);

  private:
      CElementSetFile& m_instance;
  };

public:
  // Results, properties exported by the element set are defined as TValueComponent
  typedef const IValueComponentBase* TValueComponent;
  typedef CHeaderToken THeaderToken;
//	typedef INodalFormat::TAxisSystem TAxisSystem;
private:
  // Export 
  const CNodalExportFormat*		m_pNodalExportFormat;
  const IElementSetExportFormat*	m_pElementSetExportFormat;
  bool m_bCoorUnitSet; // wjrx mantis 3336

  IElementSet* m_pElementSet;
  IPointSet* m_pPointSet;
  bool m_bWeighting;

  QSharedPointer <CNodalImportFormat>   m_pNodalFormat;
  QSharedPointer <CElementImportFormat> m_pElementFormat;
  CResultImportFormat*   m_pResultFormat;
  CWellPathInputImportFormat* m_pWellPathInputFormat;
  
  // If you don't want to write the header with all the '***' set this bool to false.
  // The 'END' statement will also not be written.
  bool m_bWriteHeader;

  CModelBase& m_model;
  IImportFormat& CurrentFormat();
  int LineLength();
  bool WriteUnit(TOutputStream& stream);
  bool WriteDepStages(TOutputStream& stream);
  bool WriteTable(const IExportFormat& format, TOutputStream& stream);

  //std::map<std::string, CGeomecTime> m_mpDepTimes;

  void ReadRow(TInputStream& stream);
  QString ReadName(TInputStream& stream);
  void    WriteName(const QString& sName, TOutputStream& stream); 

  QString m_strExportDelimiter;
  CGraphNode::TNodeVec* m_created_objects;
public:

// Contruction / Destruction
  CElementSetFile(CModelBase& model);	
  CElementSetFile(CModelBase& model, CGraphNode::TNodeVec& created_objects);	
  CElementSetFile(const CNodalExportFormat& export_format);	
  CElementSetFile(const IElementSetExportFormat& export_format);	
  CElementSetFile(CResultImportFormat &result_format);
  CElementSetFile(CWellPathInputImportFormat &well_format);

  virtual ~CElementSetFile();

  //##ModelId=3C74E3E7034E
  virtual bool OnParseFail(TInputStream& stream, const QString& sToken);
  virtual bool IgnoreRead(TInputStream& stream, const QString& sToken);

  // Access to element and point set we're reading or writing
  bool IsElementSet() const;
  const IElementSet& ElementSet() const;
  IElementSet& ElementSet();
  bool IsPointSet() const;
  const IPointSet& PointSet() const;
  IPointSet& PointSet();	

  void SetDelimiter(const QString &delimiter) { m_strExportDelimiter = delimiter; }
  void WriteHeader(bool bWrite) { m_bWriteHeader = bWrite; }

  // Axis system
//	TAxisSystem AxisSystem() const;

protected:
  virtual bool OnRead(TInputStream& stream);
  virtual long SavedItems() const;

  bool ReadHeader();

  // virtual pointset and elementset creation (for own types)
  virtual CElementSet* OnCreateElementSet(const QString& sName, CFemAppModel& model, CQuantity::UNIT coordinate_unit, CQuantity::UNIT value_unit) const;
  virtual CPointSet* OnCreatePointSet(const QString& sName, CFemAppModel& model, int nValueSetSize, IPointSet::DIMENSION dim, bool bCreateValueSetForCoordinates) const;

  // Parse events
  bool OnResults(TInputStream& stream, const QString& sToken);
  bool OnElementSet(TInputStream& stream, const QString& sToken);
  bool OnSurface(TInputStream& stream, const QString& sToken);
  bool OnPointSet(TInputStream& stream, const QString& sToken);
  bool OnPoint_Set(TInputStream& stream, const QString& sToken);
  bool OnProperty(TInputStream& stream, const QString& sToken);
  bool OnElementTrailer(TInputStream& stream, const QString& sToken);
  bool OnCoor(TInputStream& stream, const QString& sToken);
  bool OnUnit(TInputStream& stream, const QString& sToken);
  bool OnElement(TInputStream& stream, const QString& sToken);
  bool OnPoints(TInputStream& stream, const QString& sToken);
  bool OnDepletionTimes(TInputStream& stream, const QString& sToken);
  bool OnEnd(TInputStream& stream, const QString& sToken);
  
  // Write functions
  virtual bool OnWrite(TOutputStream& stream);

  const CModelBase& Model() const;
  CModelBase& Model();
};


#endif // !defined(AFX_PROPERTYFILE_H__438B853D_25C0_462E_AF3F_56AB52ED48CB__INCLUDED_)
