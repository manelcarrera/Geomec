#ifndef _RPNUNITCONVERTER_H_
#define _RPNUNITCONVERTER_H_

#include "rpnobject.h"

namespace rpn {

class CRpnUnitConverter : public CRpnObject {
public:
  // DON'T change the order, don't prepend/insert, only append for new converions
  typedef enum { UNKNOWN = 0, FT_M, M_FT, PPG_KGPM3, KGPM3_PPG, C_K, K_C, F_C, C_F, PSI_MPA, MPA_PSI } TConversion;

public:
  CRpnUnitConverter();
  CRpnUnitConverter(CRpnStack &stack, TConversion conversion);
  CRpnUnitConverter(const CRpnUnitConverter &rhs);

  virtual CRpnObject *Clone(CRpnStack &NewStack) const;

  // Value
  virtual TValue Value(const geo::IPoint &pt, UNIT unit) const;
  virtual TValueVec Value(const geo::IElement &el, UNIT unit) const;
  virtual QString Formula() const;
  virtual void Clear(CRpnStack &stack);

  // Save functions
  virtual void SaveStream(std::stringstream &stream);
  virtual void LoadStream(std::stringstream &stream, CStreamVersion &version, CRpnStack &stack);
  virtual eObjectType ObjectType() const;
  virtual bool Recursive(TParentSet stParent = TParentSet()) const;

  virtual bool Defined() const;
  virtual bool exists() const;

  virtual void CollectUsedObjects(TObjectSet &stObjects) const;

  static std::vector<std::pair<unsigned int, QString>> Collect();

  virtual CRpnObject *GetExpandedRpnObject(rpn::CRpnStack &targetstack, std::list<std::string> &lstMessages) const;

private:
  TValue Operation(const TValue &value) const;

private:
  TConversion m_conversion;
  CRpnObject *m_pOperand;
};

} // namespace rpn

#endif // _RPNUNITCONVERTER_H_
