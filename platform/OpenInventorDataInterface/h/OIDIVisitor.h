#ifndef _OIDI_VISITOR_H__
#define _OIDI_VISITOR_H__

#include "DispatchVisitorBase.h"
#include "DrawVisitorBase.h"
#include "OIDISet.h"
#include "OIDIMesh.h"

namespace geo
{
  class IMesh;
}


class OIDIVisitor : public CDrawVisitorBase
{
public:

  OIDIVisitor(const COpenGLNode *openGLNode, OIDISet *set, OIDISet::Node *ifNode) : m_OpenGLNode(openGLNode), m_Set(set), m_Node(ifNode) {}

  virtual bool HandleElementSet(const geo::IElementSet &ElementSet);
  virtual bool HandleMesh(const geo::IMesh &Mesh);
  virtual bool HandleVolume(const geo::IVolume &Volume);
  virtual bool HandleSurface(const geo::ISurface &Surface);
  virtual bool HandlePolyLine(const geo::IPolyLine &PolyLine);
  virtual bool HandleArray(const geo::IArray &Array);
  virtual bool HandleBox(const geo::IBox &Box);
  virtual bool HandlePoint(const geo::IPoint &Point);

  virtual bool HandleLabelPoint(const CLabelPoint &LabelPoint);
  virtual bool HandleSymbol(const ISymbol &Symbol);

private:

  class OIDIVolumeVisitor : public geo::CVisitorBase
  {
  public:
  OIDIVolumeVisitor(std::vector<OIDIVolumeCell>& cells) : m_Cells(cells) {}

  virtual bool HandleBody(const geo::IBody &body);


  private:
  std::vector<OIDIVolumeCell> &m_Cells;
  };

  class OIDISurfaceVisitor : public geo::CVisitorBase
  {
  public:
  OIDISurfaceVisitor(std::vector<OIDISurfaceCell>& cells) : m_Cells(cells) {}

  virtual bool HandleFace(const geo::IFace &face);
  virtual bool HandleInterface(const geo::IInterfaceElement &element);

  private:
  std::vector<OIDISurfaceCell> &m_Cells;
  };

  class OIDILineVisitor : public geo::CVisitorBase
  {
  public:
  OIDILineVisitor(std::vector<OIDILineCell>& cells) : m_Cells(cells) {}

  virtual bool HandleLine(const geo::ILine &line);


  private:
  std::vector<OIDILineCell> &m_Cells;
  };


  const COpenGLNode *m_OpenGLNode;
  OIDISet *m_Set;
  OIDISet::Node *m_Node;

};

geo::CPtrArray<geo::IPoint> * CreateArray( const geo::IPoint &Point );

#endif