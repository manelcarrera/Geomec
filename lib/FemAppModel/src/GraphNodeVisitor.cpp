#include "GraphNodeVisitor.h"

#include "GraphNode.h"
#include "FemAppModel.h"
#include "GraphEntry.h"
#include "ColorEntry.h"
#include "ColorScaleEntry.h"
#include "StorageNode.h"
#include "OpenGLNodeBase.h"
#include "OpenGLComposite.h"
#include "openglnode.h"
#include "colornode.h"
#include "IColorScaleNode.h"
#include "ivaluecomponent.h"
#include "ivaluecomposite.h"
#include "unitnode.h"

namespace graphnode {

///// IConstVisitor
IConstVisitor::IConstVisitor()
{
}

IConstVisitor::~IConstVisitor()
{
}

bool IConstVisitor::VisitGraphNode(const CGraphNode& node)
{
  return VisitDefault(node);
}

bool IConstVisitor::VisitFemAppModel(const CFemAppModel& model)
{
  return VisitGraphNode(model);
}

bool IConstVisitor::VisitGraphEntry(const CGraphEntry& entry)
{
  return VisitGraphNode(entry);
}

bool IConstVisitor::VisitColorEntry(const CColorEntry& entry)
{
  return VisitGraphEntry(entry);
}

bool IConstVisitor::VisitIColorScaleEntry(const IColorScaleEntry& entry)
{
  return VisitGraphEntry(entry);
}

bool IConstVisitor::VisitStorageNode(const CStorageNode& node)
{
  return VisitGraphNode(node);
}

bool IConstVisitor::VisitOpenGLNodeBase(const COpenGLNodeBase& node)
{
  return VisitStorageNode(node);
}

bool IConstVisitor::VisitOpenGLComposite(const COpenGLComposite& composite)
{
  return VisitOpenGLNodeBase(composite);
}

bool IConstVisitor::VisitOpenGLNode(const COpenGLNode& node)
{
  return VisitOpenGLNodeBase(node);
}

bool IConstVisitor::VisitColorNode(const CColorNode& node)
{
  return VisitOpenGLNode(node);
}

bool IConstVisitor::VisitIColorScaleNode(const IColorScaleNode& node)
{
  return VisitStorageNode(node);
}

bool IConstVisitor::VisitIValueComponentBase(const IValueComponentBase& component)
{
  return VisitStorageNode(component);
}

bool IConstVisitor::VisitIValueComposite(const IValueComposite& composite)
{
  return VisitStorageNode(composite);
}

bool IConstVisitor::VisitUnitNode(const CUnitNode& node)
{
  return VisitGraphNode(node);
}


///// IVisitor
IVisitor::IVisitor()
{
}

IVisitor::~IVisitor()
{
}

bool IVisitor::VisitGraphNode(CGraphNode& node)
{
  return VisitDefault(node);
}

bool IVisitor::VisitFemAppModel(CFemAppModel& model)
{
  return VisitGraphNode(model);
}

bool IVisitor::VisitGraphEntry(CGraphEntry& entry)
{
  return VisitGraphNode(entry);
}

bool IVisitor::VisitColorEntry(CColorEntry& entry)
{
  return VisitGraphEntry(entry);
}

bool IVisitor::VisitIColorScaleEntry(IColorScaleEntry& entry)
{
  return VisitGraphEntry(entry);
}

bool IVisitor::VisitStorageNode(CStorageNode& node)
{
  return VisitGraphNode(node);
}

bool IVisitor::VisitOpenGLNodeBase(COpenGLNodeBase& node)
{
  return VisitStorageNode(node);
}

bool IVisitor::VisitOpenGLComposite(COpenGLComposite& composite)
{
  return VisitOpenGLNodeBase(composite);
}

bool IVisitor::VisitOpenGLNode(COpenGLNode& node)
{
  return VisitOpenGLNodeBase(node);
}

bool IVisitor::VisitColorNode(CColorNode& node)
{
  return VisitOpenGLNode(node);
}

bool IVisitor::VisitIColorScaleNode(IColorScaleNode& node)
{
  return VisitStorageNode(node);
}

bool IVisitor::VisitIValueComponentBase(IValueComponentBase& component)
{
  return VisitStorageNode(component);
}

bool IVisitor::VisitIValueComposite(IValueComposite& composite)
{
  return VisitStorageNode(composite);
}

bool IVisitor::VisitUnitNode(CUnitNode& node)
{
  return VisitGraphNode(node);
}


} // namespace graphnode