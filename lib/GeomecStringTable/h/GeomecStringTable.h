#ifndef _GeomecStringTable_h_
#define _GeomecStringTable_h_

#include <qstring.h>
#include <vector>

class CValueTypeIdContainer {
private:
  static CValueTypeIdContainer *obj; // singleton
  CValueTypeIdContainer() { ; }
  std::vector<int> m_vcID;

public:
  static CValueTypeIdContainer *instance() {
    if (obj == 0)
      obj = new CValueTypeIdContainer;
    return obj;
  }
  void add(int id) { m_vcID.push_back(id); }
  size_t size() const { return m_vcID.size(); }
  int get(int id) const { return m_vcID[id]; }
};

const char *getStringTableEntry(unsigned int stringTableEntry);
// QString getStringTableEntry(unsigned int stringTableEntry);

#endif // _GeomecStringTable_h_
