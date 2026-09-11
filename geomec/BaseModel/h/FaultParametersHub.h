#ifndef _FaultParametersHub_h_
#define _FaultParametersHub_h_

class CHorizonBase;

class CFaultParametersHub
{
  public:
  CFaultParametersHub(CHorizonBase& horizonBase);

  void attributes();
  bool hasAttributes() const;

  private:
  CFaultParametersHub(const CFaultParametersHub& rhs);
  CFaultParametersHub& operator = (const CFaultParametersHub& rhs);

  CHorizonBase& m_horizonBase;
};

#endif  // _FaultParametersHub_h_
