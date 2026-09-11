#if !defined(__LOCAL_RESULT_H__)
#define __LOCAL_RESULT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>

class IValueComposite;

typedef std::pair<const IValueComposite *, int> TResultComponent; // The integer represents the index component

// The less operator of the TResultComponents
class CResultComponentLess {
public:
  bool operator()(const TResultComponent &c1, const TResultComponent &c2) const;
};

typedef std::set<TResultComponent, CResultComponentLess> TResultComponentSet;

#endif