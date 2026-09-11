#ifndef _WELLCASINGRTCI_H_
#define _WELLCASINGRTCI_H_

#include "ResultRegister.h"

class CWellCasingModel;

namespace geo {
  class CHexahedron;
}

#include "colornode.h"
#include "IPolyLine.h"
#include "Hexahedron.h"

// The RTCI (Real Time Compaction Imaging) is a wire winding spirally down around the casing
// It can measure axial strain (in the wire).
// This class models the behaviour of an RTCI wire

class CWellCasingRTCI : public CColorNode
{
public:
  // winding direction while going down (wire coordinate becomes higher), looking down
  // so effectively wiring direction around positive Z axis
  typedef enum
  {
    RD_CLOCKWISE,
    RD_ANTICLOCKWISE
  } TRtciDir;

  class CSegmentInfo : public geo::ILine
  {
  public:
    CSegmentInfo(int nIndex, const geo::CHexahedron& hexa, double dStart, double dEnd, const geo::IPoint& ptStart, const geo::IPoint& ptEnd, double dStartAHD, double dEndAHD);
    ~CSegmentInfo();

    double Start() const;
    double End() const;

    double StartAHD() const;
    double EndAHD() const;

    virtual const geo::IPoint& Point(int nIndex) const;
    virtual int PointIndex(int nIndex) const;

    geo::CValue AxialStrain(const geo::IPoint& point, const CStrainTensorValueSet& strain) const;

    virtual size_t Order() const;

  private:
    void AxialStrain(const CStrainTensorValueSet& strain, const geo::CPoint& pt, const geo::IElement::TDoubleVec& vcIsoCoords, geo::CValue& val) const;

  private:
    int m_nIndex;
    const geo::CHexahedron* m_pHexa;
    geo::IElement::TDoubleVec m_vcStartShapeFunction;
    geo::IElement::TDoubleVec m_vcEndShapeFunction;
    double m_dStart;
    double m_dEnd;
    geo::CElementPoint m_ptStart;
    geo::CElementPoint m_ptEnd;
    double m_dStartAHD;
    double m_dEndAHD;

    mutable geo::CVector* m_pVecDirection;
    mutable geo::CVector* m_pVecTangent;
  };

public:
  CWellCasingRTCI(CFemAppModel& model);
  CWellCasingRTCI(const QString& strName, CWellCasingModel& model);
  CWellCasingRTCI(const CWellCasingRTCI& rhs);
  virtual ~CWellCasingRTCI();

  CWellCasingRTCI& operator=(const CWellCasingRTCI& rhs);
  bool operator==(const CWellCasingRTCI& rhs) const;

  // angles are in degrees
  void Angle(double dAngle);
  double Angle() const;
  void StartAzimuth(double dAzimuth);
  double StartAzimuth() const;
  void WindingDirection(TRtciDir dir);
  TRtciDir WindingDirection() const;

  // angles are in radians
  double AzimuthAt(double dAHD) const; // always right-handed (N->E), in radians!
  double AzimuthAt(int nStep) const;
  geo::CPoint PointAt(double dAHD) const;
  double AHDAt(int nStep, int nAddCycles) const;
  geo::CPoint PointAt(int nStep, int nAddCycles) const;

  // the 'radius' of the n-corner shape of the mesh at the given azimuth (in radians)
  double RadiusAt(double dAzimuth) const;

  // the outer radius of the casing (max. radius of the spiral)
  double OuterRadius() const;

  double Rate() const; // the distance between two windings
  double Length() const;
  const geo::IPolyLine& PolyLine() const;

  size_t SegmentSize() const;

	virtual int DisplayListSize() const;
	virtual const geo::IObject& DisplayList(int nIndex) const;

	virtual geo::CPoint Min() const;
	virtual geo::CPoint Max() const;

  virtual unsigned int IconId() const;
  virtual unsigned int TypeId() const;
  virtual void OnNeighbourModified(const CGraphNode& node, enum ModifiedHint uHint);

  virtual long SavedItems() const;
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);
  virtual void LoadStream(TSTREAM& stream, CStreamVersion& version, TPROGRESS& progress);

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingRTCI);

private:
  CWellCasingModel& CasingModel();
  const CWellCasingModel& CasingModel() const;
  void GenerateCache() const;
  void InvalidateCache();
  geo::CVector DirectionAt(double dAHD) const;
  const geo::IObject& LineArray() const;

private:
  class CPolyLine : public geo::IPolyLine
  {
  public:
    CPolyLine(const CWellCasingRTCI& rtci);

    virtual int LineSize() const;
    virtual const geo::ILine& Line(int Index) const;

    virtual void Swap(int nIndex1, int nIndex2);
    virtual void AssertValid() const;
    virtual std::vector<int> Nodes(const geo::IElement &element) const;

  protected:
    virtual geo::IPoint& PointAt(int nIndex);

  private:
    const CWellCasingRTCI& m_rtci;
  };

  friend class CPolyLine;

  double m_dAngle;
  double m_dAzimuth;
  TRtciDir m_WindingDirection;
  mutable CPolyLine* m_pPolyLine;
  mutable geo::CPtrArray<geo::ILine>* m_pLines;
  mutable geo::CPtrArray<geo::CHexahedron> m_arHexas;

  typedef std::vector<CSegmentInfo*> TSegmentVec;
  mutable TSegmentVec m_vcSegments;
};

class CWellCasingRTCIEntry : public CStorageNodeEntry<CWellCasingRTCI>
{
public:
  CWellCasingRTCIEntry(CWellCasingModel& model);
	void CreateRTCI();	

  ACCEPT_GEOMECMODELVISITORS(VisitWellCasingRTCIEntry);
};

#endif // _WELLCASINGRTCI_H_