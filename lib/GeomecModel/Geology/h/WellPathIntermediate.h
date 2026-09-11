#ifndef _WELLPATHINTERMEDIATE_H_
#define _WELLPATHINTERMEDIATE_H_

#include "WellPath.h"

class CCachingIntermediate : public QObject, public CWellPath::IIntermediate
{
  Q_OBJECT

public:
  CCachingIntermediate();
  virtual ~CCachingIntermediate();

  virtual void Detach();
  virtual void AddSection(well::IWellSection& section,
                          SECTION_TYPE type,
                          QRgb color,
                          int iLineWidth,
                          bool bVisible,
                          const QString& strDescription);
  virtual void AddPoint(well::CWellPoint& point,
            QRgb color,
            int iPointSize,
            bool bVisible);
  virtual void Color(QRgb color);
  virtual void RemoveAllPoints();
  virtual void SetSectionDescription(well::IWellSection& section, const QString& strDescription);
  virtual bool IsVisible() const;
  virtual void OnNeighbourDeleted(const CGraphNode& node);
  virtual bool OnNewNeighbour(const CGraphNode& node); // returns whether update must be triggered
  virtual void DrawInfo(bool bDraw);
  virtual bool DrawInfo() const;

//  void Clear();

  void SetIntermediate(CWellPath::IIntermediate* pIntermediate);

private:
  void UpdateIntermediate(IIntermediate& intermediate);

private slots:
  void OnSectionDestroyed(const well::IWellSection& section);
  void OnPointDestroyed(const well::CWellPoint& wellpoint);

private:
  struct SectionInfo
  {
  well::IWellSection* pSection;
  SECTION_TYPE type;
  QRgb color;
  int linewidth;
  bool visible;
  QString description;
  };

  typedef std::list<SectionInfo> TSectionInfoList;
  TSectionInfoList m_lstSectionInfo;

  struct PointInfo
  {
  well::CWellPoint* pPoint;
  QRgb color;
  int pointsize;
  bool visible;
  };

  typedef std::list<PointInfo> TPointInfoList;
  TPointInfoList m_lstPointInfo;

  bool m_bHasColor;
  QRgb m_color;

  bool m_bHasDrawInfo;
  bool m_bDrawInfo;

  CWellPath::IIntermediate* m_pIntermediate;
};

#endif // _WELLPATHINTERMEDIATE_H_
