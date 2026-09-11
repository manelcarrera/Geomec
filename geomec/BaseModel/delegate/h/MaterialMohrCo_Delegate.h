#ifndef _MaterialMohrCo_Delegate_h_
#define _MaterialMohrCo_Delegate_h_

class CMaterialMohrCo;
class CMaterialMCCohesionHard1;
class CMaterialMCCohesionHard2;
class CMaterialMCCohesionHard3;
class CMaterialMCFrictionHard1;
class CMaterialMCFrictionHard2;
class CMaterialMCFrictionHard3;

#include "IMaterialRock_Delegate.h"

class CMaterialMohrCo_Delegate : public IMaterialRock_Delegate {
public:
  CMaterialMohrCo_Delegate(CMaterialMohrCo *materialMohrCo);

private:
  CMaterialMohrCo_Delegate(const CMaterialMohrCo_Delegate &rhs);
  CMaterialMohrCo_Delegate &operator=(const CMaterialMohrCo_Delegate &rhs);

  CMaterialMohrCo *m_materialMohrCo;

  REGISTER_DELEGATE(CMaterialMohrCo, CMaterialMohrCo_Delegate);
};

class CMaterialMCCohesionHard1_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCCohesionHard1_Delegate(CMaterialMCCohesionHard1 *materialMCCohesionHard1);

private:
  CMaterialMCCohesionHard1_Delegate(const CMaterialMCCohesionHard1_Delegate &rhs);
  CMaterialMCCohesionHard1_Delegate &operator=(const CMaterialMCCohesionHard1_Delegate &rhs);

  CMaterialMCCohesionHard1 *m_materialMCCohesionHard1;

  REGISTER_DELEGATE(CMaterialMCCohesionHard1, CMaterialMCCohesionHard1_Delegate);
};

class CMaterialMCCohesionHard2_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCCohesionHard2_Delegate(CMaterialMCCohesionHard2 *materialMCCohesionHard2);

private:
  CMaterialMCCohesionHard2_Delegate(const CMaterialMCCohesionHard2_Delegate &rhs);
  CMaterialMCCohesionHard2_Delegate &operator=(const CMaterialMCCohesionHard2_Delegate &rhs);

  CMaterialMCCohesionHard2 *m_materialMCCohesionHard2;

  REGISTER_DELEGATE(CMaterialMCCohesionHard2, CMaterialMCCohesionHard2_Delegate);
};

class CMaterialMCCohesionHard3_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCCohesionHard3_Delegate(CMaterialMCCohesionHard3 *materialMCCohesionHard3);

private:
  CMaterialMCCohesionHard3_Delegate(const CMaterialMCCohesionHard3_Delegate &rhs);
  CMaterialMCCohesionHard3_Delegate &operator=(const CMaterialMCCohesionHard3_Delegate &rhs);

  CMaterialMCCohesionHard3 *m_materialMCCohesionHard3;

  REGISTER_DELEGATE(CMaterialMCCohesionHard3, CMaterialMCCohesionHard3_Delegate);
};

class CMaterialMCFrictionHard1_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCFrictionHard1_Delegate(CMaterialMCFrictionHard1 *materialMCFrictionHard1);

private:
  CMaterialMCFrictionHard1_Delegate(const CMaterialMCFrictionHard1_Delegate &rhs);
  CMaterialMCFrictionHard1_Delegate &operator=(const CMaterialMCFrictionHard1_Delegate &rhs);

  CMaterialMCFrictionHard1 *m_materialMCFrictionHard1;

  REGISTER_DELEGATE(CMaterialMCFrictionHard1, CMaterialMCFrictionHard1_Delegate);
};

class CMaterialMCFrictionHard2_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCFrictionHard2_Delegate(CMaterialMCFrictionHard2 *materialMCFrictionHard2);

private:
  CMaterialMCFrictionHard2_Delegate(const CMaterialMCFrictionHard2_Delegate &rhs);
  CMaterialMCFrictionHard2_Delegate &operator=(const CMaterialMCFrictionHard2_Delegate &rhs);

  CMaterialMCFrictionHard2 *m_materialMCFrictionHard2;

  REGISTER_DELEGATE(CMaterialMCFrictionHard2, CMaterialMCFrictionHard2_Delegate);
};

class CMaterialMCFrictionHard3_Delegate : public CMaterialMohrCo_Delegate {
public:
  CMaterialMCFrictionHard3_Delegate(CMaterialMCFrictionHard3 *materialMCFrictionHard3);

private:
  CMaterialMCFrictionHard3_Delegate(const CMaterialMCFrictionHard3_Delegate &rhs);
  CMaterialMCFrictionHard3_Delegate &operator=(const CMaterialMCFrictionHard3_Delegate &rhs);

  CMaterialMCFrictionHard3 *m_materialMCFrictionHard3;

  REGISTER_DELEGATE(CMaterialMCFrictionHard3, CMaterialMCFrictionHard3_Delegate);
};

#endif // _MaterialMohrCo_Delegate_h_
