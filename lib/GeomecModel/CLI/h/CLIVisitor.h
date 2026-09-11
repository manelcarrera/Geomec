#ifndef _CLIVISITOR_H_
#define _CLIVISITOR_H_

#include "GeomecModelVisitor.h"

class QString;

class CCLIVisitor : public IGeomecModelVisitor {
public:
  CCLIVisitor(const QString &cmd);

  CGraphNode *selectedNode();

  virtual bool VisitDefault(CGraphNode &node);

  virtual bool VisitModelBase(CModelBase &model);
  virtual bool VisitHexaFormationEntry(CHexaFormationEntry &entry);
  virtual bool VisitHexaFormation(CHexaFormation &formation);
  virtual bool VisitPressure(CPressure &pressure);

private:
  // by reference command
  bool consider(CGraphNode &node, const QString &refcmd);
  // by name
  bool consider(CGraphNode &node);

private:
  const QString &m_cmd;
  CGraphNode *m_selectedNode;
};

#endif // _CLIVISITOR_H_
