#include "CLICommandVisitor.h"

#include "HexaFormation.h"

CCLICommandVisitor::CCLICommandVisitor(const TStringList& commands, TStringList::const_iterator itCommand)
: m_commands(commands),
  m_itCommand(itCommand)
{
}

bool CCLICommandVisitor::VisitDefault(CGraphNode& /*node*/)
{
  return false;
}

bool CCLICommandVisitor::VisitHexaFormation(CHexaFormation& formation)
{
  if(*m_itCommand == "TYPE")
  {
  TStringList::const_iterator it = m_itCommand;
  if(++it != m_commands.end())
  {
      if(*it == "NOTSPECIFIED")
    formation.FormationType(CFormationBase::FT_NOTSPECIFIED);
      else if(*it == "SHALE")
    formation.FormationType(CFormationBase::FT_SHALE);
      else if(*it == "SAND")
    formation.FormationType(CFormationBase::FT_SAND);
      else if(*it == "CHALK")
    formation.FormationType(CFormationBase::FT_CHALK);
      else if(*it == "SALT")
    formation.FormationType(CFormationBase::FT_SALT);
      else if(*it == "LIMESTONE")
    formation.FormationType(CFormationBase::FT_LIMESTONE);
      else if(*it == "SANDSTONE")
    formation.FormationType(CFormationBase::FT_SANDSTONE);
      else
    return false;

      return true;
  }
  }

  return IGeomecModelVisitor::VisitHexaFormation(formation);
}
