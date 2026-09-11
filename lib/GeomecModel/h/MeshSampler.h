#ifndef _MESHSAMPLER_H_
#define _MESHSAMPLER_H_

class CModelBase;
class CMeshBase;

#include "CoordinateMap.h"
#include "colornode.h"
#include "GeomecModelVisitor.h"

class CPointSet;

namespace geo {
  class IMesh;
  class IPoint;
}

class ITargetPointsetSource
{
public:
  ITargetPointsetSource();
  virtual ~ITargetPointsetSource();

  virtual int PointSize() const = 0;
  virtual const geo::IPoint& Point(int nIndex) const = 0;
  virtual const double* MaximumDepth() const;
};

class CHexaMesh;
class CGridPointsetSource : public ITargetPointsetSource
{
public:
  CGridPointsetSource(const geo::IPoint& ptCorner1, const geo::IPoint& ptCorner2, int nPointsNorthing, int nPointsEasting);
  CGridPointsetSource(const CHexaMesh& hexamesh);

  virtual int PointSize() const;
  virtual const geo::IPoint& Point(int nIndex) const;

private:
  std::vector<geo::CPoint> m_vcPoints;
};

class IPointSet;
class CPointsetSource : public ITargetPointsetSource
{
public:
  CPointsetSource(const IPointSet& pointset);
  CPointsetSource(const IPointSet& pointset, double dMaxDepth);

  virtual int PointSize() const;
  virtual const geo::IPoint& Point(int nIndex) const;
  virtual const double* MaximumDepth() const;

private:
  const IPointSet& m_pointset;
  double m_dMaxDepth;
  double *m_pMaxDepth; // points to m_dMaxDepth if a max depth was supplied in the contructor
};

class CPillarPos : public geo::IPoint
{
public:
  CPillarPos(double dX, double dY);
  CPillarPos(const CPillarPos& rhs);

  CPillarPos& operator=(const CPillarPos& rhs);

  // position of the pillar
  virtual const double& X() const;
  virtual const double& Y() const;

  // always return (reference to) 0
  virtual const double& Z() const;

  // not allowed
  virtual void X(const double& dX);
  virtual void Y(const double& dY);
  virtual void Z(const double& dZ);

	virtual void AssertValid() const;
	virtual bool Empty() const;

private:
  double m_dX;
  double m_dY;
  static double m_dZ; // = 0
};

class CPillar
{
public:
  class CPillarPoint;
  class CConstIterator
  {
    friend class CPillarPoint;

  public:
    CConstIterator();
    CConstIterator(const CPillar& pillar, std::map<double, const geo::IBody*>::const_iterator it);
    CConstIterator(const CConstIterator& rhs);
    CConstIterator& operator=(const CConstIterator& rhs);
    bool operator==(const CConstIterator& rhs);
    bool operator!=(const CConstIterator& rhs);
    CConstIterator& operator++(); // pre-increment
    CConstIterator operator++(int); // post-increment
    CConstIterator& operator--(); // pre-decrement
    CConstIterator operator--(int); // post-decrement

    std::pair<CPillarPoint, const geo::IBody*> operator*();

  private:
    const CPillar* m_pPillar;
    std::map<double, const geo::IBody*>::const_iterator m_it;
  };

  class CPillarPoint : public geo::IPoint
  {
  public:
    CPillarPoint(const CConstIterator& iter);

    virtual const double& X() const;
    virtual const double& Y() const;
    virtual const double& Z() const;

    // not allowed
    virtual void X(const double& dX);
    virtual void Y(const double& dY);
    virtual void Z(const double& dZ);

	  virtual void AssertValid() const;
	  virtual bool Empty() const;

  private:
    double m_dX;
    double m_dY;
    double m_dZ;
  };

  friend class CConstIterator;
  typedef CConstIterator const_iterator;
  typedef size_t size_type;

public:
  CPillar();
  CPillar(const CPillarPos& pos);

  CPillarPos Position() const;
  const_iterator begin() const;
  const_iterator end() const;
  size_type size() const;

  std::pair<const_iterator, bool> insert(double dZ);
  void DetectElements(const geo::IMesh& mesh, bool bInclude);

  long SavedItems() const;
  void SaveStream(CStorageNode::TSTREAM& stream, CStorageNode::TPROGRESS& progress);
  void LoadStream(CStorageNode::TSTREAM& stream, CStorageNode::TSTREAMVERSION& version, CStorageNode::TPROGRESS& progress, const geo::IMesh& mesh);

private:
  double m_dX;
  double m_dY;
  typedef std::map<double, const geo::IBody*> TIntervalMap;
  TIntervalMap m_mpInterval; // first element is NULL!
};

class CPillarMap : public CColorNode
{
public:
  typedef geo::CCoordinateMap<CPillarPos, CPillar> TPillarMap;
  typedef TPillarMap::const_iterator const_iterator;

public:
  CPillarMap(CFemAppModel& model);
  CPillarMap(const QString& strName, CModelBase& model);
  ~CPillarMap();

  const CPillarPos& AddPillar(const geo::IPoint& position);
  void AddPoint(const CPillarPos& pos, double dZ);

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

	virtual bool Empty() const;
	virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

	virtual unsigned int IconId() const;
	virtual unsigned int TypeId() const;

  void DetectElements(const geo::IMesh& mesh, bool bInclude);

  const_iterator begin() const;
  const_iterator end() const;

  size_t size() const;

  ACCEPT_GEOMECMODELVISITORS(VisitPillarMap);

private:
  void InvalidateDrawCache() const;
  void BuildDrawCache() const;

private:
  TPillarMap m_mpPillars;
  mutable geo::CPtrArray<geo::IPoint> m_arDrawPoints;
  mutable geo::CPtrArray<geo::ILine> m_arDrawLines;
};

class CMeshSampler
{
public:
  CMeshSampler(const ITargetPointsetSource& source, const CMeshBase& mesh);

  // ownership of the resulting pointset is transferred to the receiver
  CPointSet* GeneratePointset(const QString& name, CModelBase& model, IProgressBase* pProgress = 0, bool bStorePointsOutsideMesh = false);

  // ownership of the resulting pillarmap is transferred to the receiver
  CPillarMap* GeneratePillarMap(const QString& name, CModelBase& model, IProgressBase* pProgress = 0, bool bInclude = false);

private:
  class IGenerator
  {
  public:
    IGenerator();
    virtual ~IGenerator();

    virtual void OnNewSourcePoint(const geo::IPoint& point) = 0;
    virtual void AddPoint(double dZ, const std::set<const geo::IElement*>& stElements) = 0;
    virtual bool AddPointsOutsideMesh() const;
  };

  class CPointsetGenerator : public IGenerator
  {
  public:
    CPointsetGenerator(CPointSet& pointset, bool bStorePointsOutsideMesh);
    virtual void OnNewSourcePoint(const geo::IPoint& point);
    virtual void AddPoint(double dZ, const std::set<const geo::IElement*>& stElements);
    virtual bool AddPointsOutsideMesh() const;

  private:
    CPointSet& m_pointset;
    bool m_bStorePointsOutsideMesh;
    const geo::IPoint* m_pCurrentPosition;
  };

  class CPillarMapGenerator : public IGenerator
  {
  public:
    CPillarMapGenerator(CPillarMap& pillarmap);
    virtual void OnNewSourcePoint(const geo::IPoint& point);
    virtual void AddPoint(double dZ, const std::set<const geo::IElement*>& stElements);

  private:
    CPillarMap& m_pillarmap;
    const CPillarPos* m_pCurrentPosition;
  };

private:
  void Generate(IGenerator& generator, IProgressBase* pProgress);

private:
  const ITargetPointsetSource& m_source;
  const CMeshBase& m_mesh;
};

#endif // _MESHSAMPLER_H_
