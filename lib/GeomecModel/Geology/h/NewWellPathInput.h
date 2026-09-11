#ifndef _NewWellPathInput_h_
#define _NewWellPathInput_h_

#include "PointSet.h"

class CNewWellPathInput : public CPointSet
{
  bool   m_bIsVertical;

  double m_dApproximateRadius;

  double m_dGlobalNorthing;
  double m_dGlobalEasting;
  double m_dGlobalTVD;

  QDate  m_BirthDate;

  geo::CPoint m_min;
  geo::CPoint m_max;

  public:
  CNewWellPathInput(CFemAppModel& model);
  CNewWellPathInput(const QString &name, CFemAppModel &model);
  CNewWellPathInput(const QString &name, CFemAppModel &model, const geo::CPoint &BasePoint);
  CNewWellPathInput(const QString &file, const QString &name, CFemAppModel &model, CQuantity::UNIT unit, IProgressBase &dlg);
  CNewWellPathInput(const CNewWellPath& wellpath);
  virtual ~CNewWellPathInput();

  static CNewWellPathInput *EnsureUniqueness(CNewWellPathInput *input);
  static const CNewWellPathInput *FindByIndex(const CModelBase& model, int nIndex);
  const CNewWellPath *WellPathInModel(const CModelBase& model) const;


  static const CNewWellPath *CreateWellPath(CNewWellPathInput *input, const CModelBase& model, std::vector<std::string> *messages = 0);


  bool IsVertical() const { return m_bIsVertical; }

  double GlobalNorthing() const { return m_dGlobalNorthing; }
  double GlobalEasting() const { return m_dGlobalEasting; }
  double GlobalTVD() const { return m_dGlobalTVD; }
  double DerElev() const { return GlobalTVD(); }

  void GlobalNorthing(double value) { m_dGlobalNorthing = value; }
  void GlobalEasting(double value) { m_dGlobalEasting = value; }
  void GlobalTVD(double value) { m_dGlobalTVD = value; }
  void DerElev(double value) { GlobalTVD(value); }

  void BirthDate(const QDate& date) { m_BirthDate = date; }
  const QDate& BirthDate() const { return m_BirthDate; }

  bool operator==(const CNewWellPathInput& rhs) const;

  virtual bool CanConnectItem(const CGraphNode& item) const;
  virtual bool ConnectItem(const CGraphNode& item);

  virtual void LoadStream(TSTREAM& stream, CStreamVersion &version, TPROGRESS& progress);
  virtual void SaveStream(TSTREAM& stream, TPROGRESS& progress);

  void InitFromPointArray(QVector<geo::CPoint>& vcPoint, bool setGlobals = true, QVector<double> *azimuth = 0, QVector<double> *inclination = 0);

  void Recalculate(double offset);
  void Recalculate(const geo::CPoint &point);
  void Recalculate(double GlobalNorthing, double GlobalEasting, double GlobalTVD);

  void FlipDepth();

  geo::CPoint min(){ return m_min; }
  geo::CPoint max(){ return m_max; }

  virtual bool CanDestroy() const;
  virtual bool Destroy();


private:
  void Init();

  void GetInterSectionPointsWithFormationSkin(const geo::ILine& line , std::set<geo::CPoint>& PointSet) const;
  void InitVerticalWell(bool bResetGlobalTVD);
  void InitVertical(double start_northing, double start_easting, double start_depth, double start_tmd, double end_depth);

  bool CheckVerticality() const;

  void CalcBB();

  CNewWellPathInput(const CNewWellPathInput& rhs);
  CNewWellPathInput& operator=(CNewWellPathInput rhs);
};

#endif  // _NewWellPathInput_h_
