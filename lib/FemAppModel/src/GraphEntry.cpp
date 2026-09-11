// GraphEntry.cpp: implementation of the CGraphEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "GraphEntry.h"
#include "FemAppModel.h"
#include "GraphNode.h"

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ##ModelId=3B653D020371
CGraphEntry::CGraphEntry(int nEntryId, unsigned int uIconId, const QString &strInstanceName, CFemAppModel &model)
    : CGraphNode(strInstanceName), m_uIconId(uIconId), m_nEntryId(nEntryId) {
  bool succeeded = model.m_mpEntry.insert(CFemAppModel::CEntryMap::value_type(nEntryId, this)).second;

  assert(succeeded);
  create(&model);
}

// ##ModelId=3B653D020374
CGraphEntry::CGraphEntry(int nEntryId, unsigned int uIconId, unsigned int uNameId, CFemAppModel &model)
    : CGraphNode(uNameId), m_uIconId(uIconId), m_nEntryId(nEntryId) {
  bool succeeded = model.m_mpEntry.insert(CFemAppModel::CEntryMap::value_type(nEntryId, this)).second;

  assert(succeeded);
  create(&model);
}

// ##ModelId=3B653D02037E
CGraphEntry::~CGraphEntry() { Model().m_mpEntry.erase(EntryId()); }

// ##ModelId=3B653D02036D
unsigned int CGraphEntry::TypeId() const { return 0; }

// ##ModelId=3B653D02036F
unsigned int CGraphEntry::IconId() const { return m_uIconId; }

int CGraphEntry::EntryId() const { return m_nEntryId; }

bool CGraphEntry::UniqueName(const QString &sName) const {
  TEntryNodeSet stNode = GraphEntryNodes();
  for (TEntryNodeSet::const_iterator it = stNode.begin(); it != stNode.end(); it++) {
    if (sName.compare((*it)->Name(), Qt::CaseInsensitive) == 0)
      return false;
  }

  return true;
}

CFemAppModel &CGraphEntry::Model() { return dynamic_cast<CFemAppModel &>(*parent()); }

const CFemAppModel &CGraphEntry::Model() const { return dynamic_cast<CFemAppModel &>(*parent()); }

QString CGraphEntry::createUniqueName(const QString &defaultName, size_t initialNumber) const {
  QString uniqueName;

  do {
    uniqueName = defaultName.arg(initialNumber++);
  } while (!UniqueName(uniqueName));

  return uniqueName;
}

std::pair<QString, QString> CGraphEntry::createUniqueNames(const QString &firstName, const QString &secondName,
                                                           size_t initialNumber) const {
  QString uniqueFirstName, uniqueSecondName;

  do {
    uniqueFirstName = firstName.arg(initialNumber);
    uniqueSecondName = secondName.arg(initialNumber++);
  } while (!UniqueName(uniqueFirstName) || !UniqueName(uniqueSecondName));

  return std::make_pair(uniqueFirstName, uniqueSecondName);
}
