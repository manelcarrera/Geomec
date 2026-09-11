#if !defined(_IFACEELMMAT_H)
#define _IFACEELMMAT_H

#include "HorizonBase.h"
#include "dimple.h"

#include "IDCElementProperty.h"

class CInterfaceElementMaterial : public dia::IMaterial
{
public:
	CInterfaceElementMaterial(const ftn_double_t& cohesion, 
		                        const ftn_double_t& friction, 
							              const ftn_double_t& dstiffnormal,
                            const ftn_double_t& dstiffshear,
						                CHorizonBase::TSlipType slip_type = CHorizonBase::USER);

	virtual bool WriteFilos(dia::IDianaRunner& diarunner) const;
	virtual bool operator<(const dia::IMaterial &rhs) const;
  double ParameterValue(unsigned int ValueTypeID) const;
	bool ValidParameterValue(unsigned int ValueTypeID) const; // wjrx mantis 2545

  // Interface for IElementProperty
  virtual int Type() const;
  virtual int WriteFilosParamSize(dia::IDianaRunner& diarunner) const;
  virtual bool WriteFilosParamName(dia::IDianaRunner& diarunner, int i, char *name) const;
  virtual void WriteFilosParamValues(dia::IDianaRunner& diarunner, double *values, int stride = 1) const;

  virtual bool isInterfaceElementMaterial() const;
  bool sameSlip(const CInterfaceElementMaterial& rhs) const;

private:
	ftn_double_t            m_cohesion;
	ftn_double_t            m_friction;
	ftn_double_t            m_dstiffnormal;
  ftn_double_t            m_dstiffshear;
	CHorizonBase::TSlipType m_slip_type;
};


#endif
