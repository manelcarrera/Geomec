#include "CLIParser.h"

#include "CLIVisitor.h"
#include "CLICommandVisitor.h"
#include "GraphNode.h"

CCLIParser::CCLIParser()
: m_selectedNode(0)
{
}

bool CCLIParser::parse(CGraphNode& root, const QString& cmd)
{
  std::list<QString> lstCmd = split(cmd);
  std::list<QString>::iterator it = lstCmd.begin();
  return recursiveParse(root, lstCmd, it);
}

CCLIParser::TStringList CCLIParser::split(const QString& cmd)
{
  TStringList lst;

  int idxLastPos = -1;

  bool bInQuote = false;

  for(int i = 0; i < cmd.size(); ++i)
  {
    const QChar& c = cmd[i];
    QString s;
    if(c == ' ')
    {
      if(!bInQuote)
      {
        s = cmd.mid(idxLastPos + 1, i - idxLastPos - 1);
        idxLastPos = i;
      }
    }
    else if(c == '"')
    {
      if(!bInQuote)
      {
        bInQuote = true;
        idxLastPos = i;
      }
      else
      {
        s = cmd.mid(idxLastPos + 1, i - idxLastPos - 1);
        bInQuote = false;
        idxLastPos = i;
      }
    }

    if(!s.isEmpty())
      lst.push_back(s);
  }

  if(bInQuote) // closing quote missing
    lst.clear();

  return lst;
}

bool CCLIParser::recursiveParse(CGraphNode& node, TStringList& lst, TStringList::iterator it)
{
  if(it == lst.end())
    return true;

  CCLIVisitor v(*it);
  node.Accept(v);
  if(v.selectedNode())
  {
    m_selectedNode = v.selectedNode();
    ++it;
    // try command
    CCLICommandVisitor cv(lst, it);
    bool bRet = node.Accept(cv);
    if(!bRet)
    {
      // try children
      for(size_t i = 0; i < node.childSize(); ++i)
        if(recursiveParse(node.childAt(i), lst, it))
          bRet = true;
    }

    return bRet;
  }

  return false;
}
