// MeshBase.h: interface for the CMeshBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHBASE_H__613A30EE_54A6_48E9_A0AF_E6E56AEAB1C5__INCLUDED_)
#define AFX_MESHBASE_H__613A30EE_54A6_48E9_A0AF_E6E56AEAB1C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFFMaterial;

#include "ElementSet.h"

class multiply
{
  double m_Multiplier;
public:
  multiply(double Multiplier) : m_Multiplier(Multiplier) {}
  double operator()(const double& x) const
  {
    return m_Multiplier * x;
  }
};

class CResultRegister;
class IFormationElementSet;
class CHorizonBase;
class CDepletionStageEntry;


class CMeshBase : public IElementSet
{
  typedef CSingleCommandTemplate<CMeshBase> TMeshBaseCommand;
  mutable bool m_bLocked;
  CResultRegister *m_pResultRegister; // pointer to the result register
  mutable std::vector<const IFormationElementSet*> m_vcElementToFormationSet; // indexed map from element index to formation
  void BuildFormationElementSetMap() const;
  bool m_bInvalidateAtUnlock;
  bool m_bGenerateSupports;

public:
  void InvalidateAtUnLock();

  // Interface of elementset always returning the current deformed mesh ...
  const geo::IElementSet &ElementSet() const; 
  geo::IElementSet &ElementSet();

  // Initial volume always returns initial undeformed mesh
  virtual const geo::IMesh& Mesh() const = 0;
  virtual geo::IMesh& Mesh() = 0;

  // Function to get a formation for a given element (member of the mesh of course)
  const CFormationBase* Formation(const geo::IElement& element) const;

  // Function to get a formation element set for a given element
  const IFormationElementSet* FormationElementSet(const geo::IElement& element) const;

  // Retrieves the slipping horizon for an interface element
  virtual const CHorizonBase* SlipHorizon(const geo::IElement& element) const = 0;

  const CResultRegister& ResultRegister() const;
  CResultRegister& ResultRegister();

  virtual bool CanInvalidateMesh() const;

  // The filos relationship between nodes and elements
  typedef std::vector<int> TNodeVec;
  typedef std::pair<const geo::IElement*, TNodeVec> TFilosElement;
  typedef std::map<unsigned int, TFilosElement> TFilosElementMap;

  // Construction
  CMeshBase(const QString& sName, CDepletionStageEntry& dep_entry);
  virtual ~CMeshBase();

  // Meshing interface
  virtual bool CanCreateMesh() const = 0;
  virtual bool IsMesh() const = 0;
  virtual void CreateMesh() = 0;
  virtual void InvalidateMesh();			// Invalidate mesh invalidate the deformed mesh ....
  virtual bool CanDestroy() const { return false; }
  virtual bool OnGridLine(const geo::IPoint& /*pt1*/, const geo::IPoint& /*pt2*/) const {return false;}

  // DisplayListing
  virtual int DisplayListSize() const;
  virtual const geo::IObject& DisplayList(int nIndex) const;

  virtual bool PointInConvexHull(const geo::IPoint& pt) const;

  // Locking
  void Lock() const;
  void Unlock();
  bool Locked() const;

  // Supports generation
  virtual void GenerateSupports() {}
  virtual void InvalidateSupports() {}

  // Functions are not in use	
  virtual int RegisterPoint( const geo::IPoint& /*point*/ ) { assert(FALSE); return -1; }
  virtual int CreateElement(const TNodeVec& /*vcIndex*/) { assert(FALSE); return -1; }

  // Graph Notification
  virtual void OnNeighbourDeleted(const CGraphNode &item);

  long SavedItemsValueSets() const;
  void SaveValueSets(TSTREAM& stream, TPROGRESS& progress);
  void LoadValueSets(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  virtual const CFFMaterial* getCFFMaterial(const geo::IElement& element,
  const CDepletionStage& depletionStage) const;

  // Get the index of the reference point in the mesh
  // An assertion fails if the mesh has not been created
  int ReferencePointIndex() const;

  virtual bool ImportedMesh() const;

  bool getGenerateSupports() const;
  void setGenerateSupports(bool bGenerateSupports);

  ACCEPT_GEOMECMODELVISITORS(VisitMeshBase);
};

#endif // !defined(AFX_MESHBASE_H__613A30EE_54A6_48E9_A0AF_E6E56AEAB1C5__INCLUDED_)
