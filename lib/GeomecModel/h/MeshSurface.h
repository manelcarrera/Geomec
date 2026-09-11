#ifndef _MESHSURFACE_H_
#define _MESHSURFACE_H_

#include "ISurface.h"

class CMeshSurface : public geo::ISurface
{
public:
  CMeshSurface();
  virtual const geo::CPtrArray<geo::IFace> FacesAt(const geo::IPoint& p) const;
  virtual const geo::CPtrArray<geo::IFace> FacesAtNode(const geo::IPoint& p) const;
  virtual const geo::IFace& Face(int nIndex) const;
  virtual int FaceSize() const;
  virtual const geo::IPoint& Point(int nIndex) const;
  virtual int PointSize() const;
  virtual std::vector<int> Nodes(const geo::IElement& element) const;

  void AddFace(const geo::IFace& face);
  virtual void reserveFaceSize(size_t size);
  void Clear();

protected:
  virtual geo::IPoint& PointAt(int nIndex);

private:
  std::vector<const geo::IFace*> m_vcFaces;
  std::vector<const geo::IPoint*> m_vcPoints;
  typedef std::map<const geo::IPoint*, std::vector<const geo::IFace*> > TPointMap;
  TPointMap m_mpPoints;
};

#endif // _MESHSURFACE_H_
