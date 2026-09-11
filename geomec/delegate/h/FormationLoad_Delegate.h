#ifndef _FormationLoad_Delegate_h_
#define _FormationLoad_Delegate_h_

#include "FormationLoadBase_Delegate.h"
#include "FormationLoad.h"
#include "IPlatform.h"
#include "GraphMainFrm.h"
#include "FemAppMainWindow.h"

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE,
  class GWC_TYPE> class CFormationLoad_Delegate :
    public CFormationLoadBase_Delegate <VALUE_TYPE>
{
public:
  CFormationLoad_Delegate(CFormationLoad <VALUE_TYPE, CONSTANT_TYPE,
    REPEATER_TYPE, GWC_TYPE> * formationLoad);

  virtual bool OnCanPaste() const;
  virtual void OnPaste();

private:
  CFormationLoad_Delegate(const CFormationLoad_Delegate& rhs);
  CFormationLoad_Delegate& operator = (const CFormationLoad_Delegate& rhs);

  CFormationLoad <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> *
    m_formationLoad;

  typedef CFormationLoad <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE>
    CFormationLoadTemplate;
  typedef CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE,
    GWC_TYPE> CFormationLoad_DelegateTemplate;

  REGISTER_DELEGATE(CFormationLoadTemplate, CFormationLoad_DelegateTemplate);
};

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE,
  class GWC_TYPE>
    CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE,
      GWC_TYPE> ::CFormationLoad_Delegate(CFormationLoad <VALUE_TYPE,
        CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> * formationLoad)
: CFormationLoadBase_Delegate <VALUE_TYPE> (formationLoad)
, m_formationLoad(formationLoad)
{
  ACTIVATE_TEMPLATE_DELEGATE(CFormationLoadTemplate,
    CFormationLoad_DelegateTemplate);
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE,
  class GWC_TYPE>
    bool CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE,
      GWC_TYPE> ::OnCanPaste() const
{
  bool bRet = false;
  IPlatform* platform = IPlatform::instance();

  platform->trace("CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, "
    "REPEATER_TYPE, GWC_TYPE> ::OnCanPaste()\n");

  CGraphMainFrm* pMainFrame = (CGraphMainFrm*) FemAppGetMainWnd();
  const CFormationLoad <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> *
    pFormationLoad = dynamic_cast <const CFormationLoad <VALUE_TYPE,
      CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> *> (pMainFrame->Paste());
  std::string s;  // dummy for CDoubleQuantity::CheckValue calls

  if (pFormationLoad)
  {
    bRet = true;

    switch (pFormationLoad->Type())
    {
      case CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE:
        {
          double dDepth = pFormationLoad->Constant().ReferenceDepth().
            Value(CQuantity::SI_UNIT);
          double dGradient = pFormationLoad->Constant().Gradient().
            Value(CQuantity::SI_UNIT);
          double dLoad = pFormationLoad->Constant().ReferenceValue().
            Value(CQuantity::SI_UNIT);

          bRet = bRet && m_formationLoad->Constant().ReferenceDepth().
            CheckValue(dDepth, s);
          bRet = bRet && m_formationLoad->Constant().Gradient().
            CheckValue(dGradient, s);
          bRet = bRet && m_formationLoad->Constant().ReferenceValue().
            CheckValue(dLoad, s);
        }
        break;
      case CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE:
        // always ok
        break;
      case CFormationLoadBase <VALUE_TYPE> ::GWC_MODE:
        {
          double dContactDepth = pFormationLoad->GWC().ContactDepth().
            Value(CQuantity::SI_UNIT);
          double dContactPressure = pFormationLoad->GWC().ContactPressure().
            Value(CQuantity::SI_UNIT);
          double dLowerGradient = pFormationLoad->GWC().LowerGradient().
            Value(CQuantity::SI_UNIT);
          double dUpperGradient = pFormationLoad->GWC().UpperGradient().
            Value(CQuantity::SI_UNIT);
          double dTransPressure = pFormationLoad->GWC().TransitionPressure().
            Value(CQuantity::SI_UNIT);
          double dTransDepth = pFormationLoad->GWC().TransitionDepth().
            Value(CQuantity::SI_UNIT);

          bRet = bRet && m_formationLoad->GWC().ContactDepth().
            CheckValue(dContactDepth, s);
          bRet = bRet && m_formationLoad->GWC().ContactPressure().
            CheckValue(dContactPressure, s);
          bRet = bRet && m_formationLoad->GWC().LowerGradient().
            CheckValue(dLowerGradient, s);
          bRet = bRet && m_formationLoad->GWC().UpperGradient().
            CheckValue(dUpperGradient, s);
          bRet = bRet && m_formationLoad->GWC().TransitionPressure().
            CheckValue(dTransPressure, s);
          bRet = bRet && m_formationLoad->GWC().TransitionDepth().
            CheckValue(dTransDepth, s);
        }
        break;
      default:
        assert(FALSE);  // unknown type
    }
  }

  return bRet;
}

template <class VALUE_TYPE, class CONSTANT_TYPE, class REPEATER_TYPE,
  class GWC_TYPE>
    void CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE,
      GWC_TYPE> ::OnPaste()
{
  IPlatform* platform = IPlatform::instance();

  platform->trace("CFormationLoad_Delegate <VALUE_TYPE, CONSTANT_TYPE, "
    "REPEATER_TYPE, GWC_TYPE>::OnPaste()\n");

  if (OnCanPaste())
  {
    CGraphMainFrm* pMainFrame = (CGraphMainFrm*) FemAppGetMainWnd();
    const CFormationLoad <VALUE_TYPE, CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> *
      pFormationLoad = static_cast <const CFormationLoad<VALUE_TYPE,
        CONSTANT_TYPE, REPEATER_TYPE, GWC_TYPE> *> (pMainFrame->Paste());

    assert(pFormationLoad);

    m_formationLoad->Type(pFormationLoad->Type());
    m_formationLoad->DistributedOnly(pFormationLoad->DistributedOnly());

    switch (pFormationLoad->Type())
    {
      case CFormationLoadBase <VALUE_TYPE> ::CONSTANT_MODE:
        {
          double dDepth = pFormationLoad->Constant().ReferenceDepth().
            Value(CQuantity::SI_UNIT);
          double dGradient = pFormationLoad->Constant().Gradient().
            Value(CQuantity::SI_UNIT);
          double dLoad = pFormationLoad->Constant().ReferenceValue().
            Value(CQuantity::SI_UNIT);

          m_formationLoad->Constant().Set(dLoad, dDepth, dGradient);
        }
        break;
      case CFormationLoadBase <VALUE_TYPE> ::REPEAT_MODE:
        // nothing to be done
        break;
      case CFormationLoadBase <VALUE_TYPE> ::GWC_MODE:
        {
          double dContactDepth = pFormationLoad->GWC().ContactDepth().
            Value(CQuantity::SI_UNIT);
          double dContactPressure = pFormationLoad->GWC().ContactPressure().
            Value(CQuantity::SI_UNIT);
          double dLowerGradient = pFormationLoad->GWC().LowerGradient().
            Value(CQuantity::SI_UNIT);
          double dUpperGradient = pFormationLoad->GWC().UpperGradient().
            Value(CQuantity::SI_UNIT);
          double dTransPressure = pFormationLoad->GWC().TransitionPressure().
            Value(CQuantity::SI_UNIT);
          double dTransDepth = pFormationLoad->GWC().TransitionDepth().
            Value(CQuantity::SI_UNIT);
          bool bTransFromPrev = pFormationLoad->GWC().TransitionFromPrevious();

          m_formationLoad->GWC().Set(dContactPressure, dTransPressure,
            dContactDepth, dTransDepth, dUpperGradient, dLowerGradient,
            bTransFromPrev);
        }
        break;
      default:
        assert(FALSE); // unknown type
    }

    m_formationLoad->Modified();
  }
}

#endif  // _FormationLoad_Delegate_h_
