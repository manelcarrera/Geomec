// GenericVolume.h: interface for the CGenericVolume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERICVOLUME_H__BF8CB128_6B6E_456D_B41D_B57B89C16D5E__INCLUDED_)
#define AFX_GENERICVOLUME_H__BF8CB128_6B6E_456D_B41D_B57B89C16D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ElementSet.h"

class CValueType;
class CGenericVolume : public IElementSet 
{
public:
  enum eElementType {HEXA = 0, TETRA, UNKNOWN, ECLIPSE};
private:
  geo::CMesh3D m_mesh;
  eElementType m_eElType;
public:
  CGenericVolume(geo::CMesh3D &Volume, const CString &sName, 
                 CGraphModel &model, 
                 CQuantity::UNIT coordinate_unit,
                 CQuantity::UNIT value_unit, eElementType type);
  CGenericVolume(const CGoCadVolume& volume, 
           CGraphModel &model, 
           CQuantity::UNIT coordinate_unit, 
           CQuantity::UNIT value_unit);
  CGenericVolume(const CString& sName, 
           CGraphModel &model, 
           CQuantity::UNIT coordinate_unit, 
           CQuantity::UNIT value_unit);
  CGenericVolume(const CGenericVolume& volume, CModelBase& model, TPROGRESS& progress, std::map<const CValueType*, CValueType*>& mpValueType);
  CGenericVolume(CGraphModel &model);
  CGenericVolume(const CGenericVolume& rhs);
  CGenericVolume(const CEclipseFile &file, CModelBase &model);
  virtual ~CGenericVolume();

  // Mesh
  geo::CMesh3D& Mesh() { return m_mesh; }

  // Assignment
  bool operator==(const CGenericVolume& rhs) const;
  CGenericVolume& operator=(const CGenericVolume& rhs);
  
  virtual const geo::IElementSet &ElementSet() const;
  virtual geo::IElementSet &ElementSet();

  // Pure virtual from CGraphNode *******************************
  virtual UINT IconId() const;
  virtual UINT TypeId() const;
  virtual CString TypeName() const;
  // ************************************************************

  // Pure virtual from COpenGLNode ******************************
  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;		
  // ************************************************************

  // Pure virtual from IPointSet ********************************
  virtual IPointSet::DIMENSION Dimension() const;
  // ************************************************************
  
  virtual BOOL Empty() const;
  virtual void ZAxis(enum Z_AXIS axis);

  int ElementPointSize() const;
  virtual void AppendContextMenu(CContextMenuInvoker &menu);

// Save and load stream
  virtual void LoadStream(TSTREAM& stream, CVersion& version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual long SavedItems() const;
  const eElementType ElementType() const {return m_eElType;}
  void ElementType(eElementType type);
  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  // Overrides from GraphNode
  virtual bool Properties();
  virtual int OnCreateElement(const std::vector<int>& vcIndex);
//	int CreateElement(const std::vector<const geo::IPoint*> &vcPoints);
};

#endif // !defined(AFX_GENERICVOLUME_H__BF8CB128_6B6E_456D_B41D_B57B89C16D5E__INCLUDED_)
