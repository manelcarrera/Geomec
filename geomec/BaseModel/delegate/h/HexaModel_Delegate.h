#ifndef _HEXAMODEL_DELEGATE_H_
#define _HEXAMODEL_DELEGATE_H_

#include "3DModel_Delegate.h"

class CHexaModel;

class CHexaModel_Delegate : public C3DModel_Delegate
{
public:
  CHexaModel_Delegate(CHexaModel* aHexaModel);

private:
  CHexaModel_Delegate(const CHexaModel_Delegate& rhs);
  CHexaModel_Delegate& operator = (const CHexaModel_Delegate& rhs);

  CHexaModel* m_HexaModel;

  REGISTER_DELEGATE(CHexaModel, CHexaModel_Delegate);
};

#endif // _HEXAMODEL_DELEGATE_H_
