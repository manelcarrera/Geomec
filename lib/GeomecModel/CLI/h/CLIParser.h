#ifndef _CLIPARSER_H_
#define _CLIPARSER_H_

#include <list>

class QString;
class CGraphNode;

class CCLIParser {
public:
  CCLIParser();

  bool parse(CGraphNode &root, const QString &cmd);

private:
  typedef std::list<QString> TStringList;
  TStringList split(const QString &cmd);
  bool recursiveParse(CGraphNode &root, TStringList &lst, TStringList::iterator it);

private:
  CGraphNode *m_selectedNode;
};

#endif // _CLIPARSER_H_
