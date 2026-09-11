#include "CLIVisitor.h"

#include "HexaFormation.h"
#include "ModelBase.h"
#include "Pressure.h"

CCLIVisitor::CCLIVisitor(const QString &cmd) : m_cmd(cmd), m_selectedNode(0) {}

CGraphNode *CCLIVisitor::selectedNode() { return m_selectedNode; }

bool CCLIVisitor::VisitDefault(CGraphNode & /*node*/) { return false; }

bool CCLIVisitor::VisitModelBase(CModelBase &model) {
  if (consider(model, "MODEL"))
    return true;

  return IGeomecModelVisitor::VisitModelBase(model);
}

bool CCLIVisitor::VisitHexaFormationEntry(CHexaFormationEntry &entry) {
  if (consider(entry, "FORMATIONS"))
    return true;

  return IGeomecModelVisitor::VisitHexaFormationEntry(entry);
}

bool CCLIVisitor::VisitHexaFormation(CHexaFormation &formation) {
  if (consider(formation))
    return true;

  return IGeomecModelVisitor::VisitHexaFormation(formation);
}

bool CCLIVisitor::VisitPressure(CPressure &pressure) {
  if (m_cmd.length() > 8 && QString::compare("PRESSURE", m_cmd.left(8), Qt::CaseInsensitive) == 0) {
    bool ok;
    int idx = m_cmd.mid(8).toInt(&ok);
    if (ok && idx == pressure.DepletionStage().Index()) {
      assert(m_selectedNode == 0);
      m_selectedNode = &pressure;
      return true;
    }
  }

  return IGeomecModelVisitor::VisitPressure(pressure);
}

bool CCLIVisitor::consider(CGraphNode &node, const QString &refcmd) {
  // case insensitive compare
  if (QString::compare(refcmd, m_cmd, Qt::CaseInsensitive) == 0) {
    assert(m_selectedNode == 0);
    m_selectedNode = &node;
    return true;
  }

  return false;
}

bool CCLIVisitor::consider(CGraphNode &node) {
  // case sensitive compare
  if (QString::compare(node.Name(), m_cmd, Qt::CaseSensitive) == 0) {
    assert(m_selectedNode == 0);
    m_selectedNode = &node;
    return true;
  }

  return false;
}
