
#include "TypeInfo.h"

TypeInfo::TypeInfo() : m_pInfo(0) {}

TypeInfo::TypeInfo(const std::type_info &tinfo) : m_pInfo(&tinfo) {}

TypeInfo::TypeInfo(const TypeInfo &rhs) : m_pInfo(rhs.m_pInfo) {}

TypeInfo &TypeInfo::operator=(const TypeInfo &rhs) {
  m_pInfo = rhs.m_pInfo;

  return *this;
}

bool TypeInfo::before(const TypeInfo &rhs) const { return (m_pInfo->before(*rhs.m_pInfo) != 0); }

const char *TypeInfo::name() const { return m_pInfo->name(); }

bool operator==(const TypeInfo &lhs, const TypeInfo &rhs) { return !lhs.before(rhs) && !rhs.before(lhs); }

bool operator!=(const TypeInfo &lhs, const TypeInfo &rhs) { return !operator==(lhs, rhs); }

bool operator<(const TypeInfo &lhs, const TypeInfo &rhs) { return lhs.before(rhs); }

bool operator<=(const TypeInfo &lhs, const TypeInfo &rhs) { return operator<(lhs, rhs) || operator==(lhs, rhs); }

bool operator>(const TypeInfo &lhs, const TypeInfo &rhs) { return rhs.before(lhs); }

bool operator>=(const TypeInfo &lhs, const TypeInfo &rhs) { return operator>(lhs, rhs) || operator==(lhs, rhs); }
