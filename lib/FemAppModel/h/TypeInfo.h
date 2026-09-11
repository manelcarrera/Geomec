#ifndef _TypeInfo_h_
#define _TypeInfo_h_

#include <typeinfo>

// from Loki (Alexandrescu)

class TypeInfo {
public:
  TypeInfo();
  TypeInfo(const std::type_info &);
  TypeInfo(const TypeInfo &);
  TypeInfo &operator=(const TypeInfo &);
  bool before(const TypeInfo &) const;
  const char *name() const;

private:
  const std::type_info *m_pInfo;
};

bool operator==(const TypeInfo &, const TypeInfo &);
bool operator!=(const TypeInfo &, const TypeInfo &);
bool operator<(const TypeInfo &, const TypeInfo &);
bool operator<=(const TypeInfo &, const TypeInfo &);
bool operator>(const TypeInfo &, const TypeInfo &);
bool operator>=(const TypeInfo &, const TypeInfo &);

#endif // _TypeInfo_h_
