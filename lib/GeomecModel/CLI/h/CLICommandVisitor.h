#ifndef _CLICOMMANDVISITOR_H_
#define _CLICOMMANDVISITOR_H_

#include "GeomecModelVisitor.h"

#include <QString>
#include <list>

class CCLICommandVisitor : public IGeomecModelVisitor {
public:
  typedef std::list<QString> TStringList;

public:
  CCLICommandVisitor(const TStringList &commands, TStringList::const_iterator itCommand);

  virtual bool VisitDefault(CGraphNode &node);
  virtual bool VisitHexaFormation(CHexaFormation &formation);

private:
  const TStringList &m_commands;
  const TStringList::const_iterator m_itCommand;
};

#endif // _CLICOMMANDVISITOR_H_
