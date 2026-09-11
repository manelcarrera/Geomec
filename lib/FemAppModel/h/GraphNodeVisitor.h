#ifndef _GRAPHNODEVISITOR_H_
#define _GRAPHNODEVISITOR_H_

class CGraphNode;
class CFemAppModel;
class CGraphEntry;
class CColorEntry;
class IColorScaleEntry;
class CStorageNode;
class COpenGLNodeBase;
class COpenGLComposite;
class COpenGLNode;
class CColorNode;
class IColorScaleNode;
class IValueComponentBase;
class IValueComposite;
class CUnitNode;

namespace graphnode {

class IConstVisitor {
protected:
  IConstVisitor();

public:
  virtual ~IConstVisitor();

  virtual bool VisitDefault(const CGraphNode &node) = 0;

  virtual bool VisitGraphNode(const CGraphNode &node);
  virtual bool VisitFemAppModel(const CFemAppModel &model);
  virtual bool VisitGraphEntry(const CGraphEntry &entry);
  virtual bool VisitColorEntry(const CColorEntry &entry);
  virtual bool VisitIColorScaleEntry(const IColorScaleEntry &entry);
  virtual bool VisitStorageNode(const CStorageNode &node);
  virtual bool VisitOpenGLNodeBase(const COpenGLNodeBase &node);
  virtual bool VisitOpenGLComposite(const COpenGLComposite &composite);
  virtual bool VisitOpenGLNode(const COpenGLNode &node);
  virtual bool VisitColorNode(const CColorNode &node);
  virtual bool VisitIColorScaleNode(const IColorScaleNode &node);
  virtual bool VisitIValueComponentBase(const IValueComponentBase &component);
  virtual bool VisitIValueComposite(const IValueComposite &composite);
  virtual bool VisitUnitNode(const CUnitNode &node);
};

class IVisitor {
protected:
  IVisitor();

public:
  virtual ~IVisitor();

  virtual bool VisitDefault(CGraphNode &node) = 0;

  virtual bool VisitGraphNode(CGraphNode &node);
  virtual bool VisitFemAppModel(CFemAppModel &model);
  virtual bool VisitGraphEntry(CGraphEntry &entry);
  virtual bool VisitColorEntry(CColorEntry &entry);
  virtual bool VisitIColorScaleEntry(IColorScaleEntry &entry);
  virtual bool VisitStorageNode(CStorageNode &node);
  virtual bool VisitOpenGLNodeBase(COpenGLNodeBase &node);
  virtual bool VisitOpenGLComposite(COpenGLComposite &composite);
  virtual bool VisitOpenGLNode(COpenGLNode &node);
  virtual bool VisitColorNode(CColorNode &node);
  virtual bool VisitIColorScaleNode(IColorScaleNode &node);
  virtual bool VisitIValueComponentBase(IValueComponentBase &component);
  virtual bool VisitIValueComposite(IValueComposite &composite);
  virtual bool VisitUnitNode(CUnitNode &node);
};

} // namespace graphnode

#endif // _GRAPHNODEVISITOR_H_
