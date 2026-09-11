#ifndef _HandleDefaultFaultParameters_h_
#define _HandleDefaultFaultParameters_h_

class CFaultFracture;

class CHandleDefaultFaultParameters {
public:
  CHandleDefaultFaultParameters(CFaultFracture *faultFracture);

  void DoDataExchange(CDataExchange *pDX);

private:
  CHandleDefaultFaultParameters(const CHandleDefaultFaultParameters &rhs);
  CHandleDefaultFaultParameters &operator=(const CHandleDefaultFaultParameters &rhs);

  CFaultFracture *m_faultFracture;

  CString m_cohesionUnit;
  CString m_frictionAngleUnit;
  double m_cohesion;
  double m_frictionAngle;
};

#endif // _HandleDefaultFaultParameters_h_
