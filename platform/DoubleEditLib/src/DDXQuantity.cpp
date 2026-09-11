// DDXQuantity.cpp: implementation of the CDDXQuantity class.
//
//////////////////////////////////////////////////////////////////////

#include "DDXQuantity.h"
#include "CDependenciesManager.h"
#include "DoubleEdit.h"
#include "GlobalMessage.h"
#include "stdafx.h"
#include <sstream>
#include <string>

#ifdef _DEBUG
#ifdef _MSC_VER
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _MSC_VER
// #define new DEBUG_NEW
#endif

#ifndef USE_UNITBRACKETS
#define USE_UNITBRACKETS true
#endif

static CString GetInvalidMaxMinString(CString sQuantityName, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us);

CQuantityReciever::CQuantityReciever(CDoubleQuantity *pQ) {
  m_bRefused = false;
  bool ret = connect(pQ, SIGNAL(OnRefuseValue(const CDoubleQuantity &, const QString &)), this,
                     SLOT(OnRefuseValue(const CDoubleQuantity &, const QString &)));
}

void CQuantityReciever::OnRefuseValue(const CDoubleQuantity &Quantity, const QString &Description) {
  m_bRefused = true;
  m_Description = Description;
}

void AFXAPI DDV_Quantity(CDataExchange *pDX, int nIDC, CDoubleQuantity *pQ, CDependenciesManager *pDM,
                         CString sQuantityName) {

  HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
  CDoubleEdit *pDE = (CDoubleEdit *)pDX->m_pDlgWnd->GetDlgItem(nIDC);

  CDoubleQuantity::UNIT us;

  if (pDM)
    us = pDM->GetUnit();
  else
    us = CDoubleQuantity::SI_UNIT;

  std::string sDependen(sQuantityName);
  if (pDX->m_bSaveAndValidate) {
    // From controls to data

    bool bWasValid = !pQ->Undefined();
    double prevValue;
    if (bWasValid) {
      // restore previous value.
      prevValue = pQ->Value(us);
    }

    if (!pDE->IsEmpty()) {
      // the edit box is not empty
      double si_val = pQ->Convert(pDE->GetValue(), QU::SI_UNIT, us);
      std::string reason;
      if (pQ->CheckValue(si_val, reason)) {
        // the max/min boundarys are ok
        std::string strDependency;

        // check dependency if pointer to manager is given
        if (pDM) {
          if (pDM->CheckQuantity(pQ, strDependency)) {
            // the dependencys are ok
            pQ->Value(si_val, QU::SI_UNIT);
            pDE->ShowValue(pDE->GetValue());
          } else {
            // there are not satisfying dependencys,show a message
            _m()->msg(strDependency.c_str(), 0, 0);
            if (bWasValid) {
              // the previous value was ok, so resore this value
              pQ->Value(prevValue, us);
              pDE->ShowValue(prevValue);
            } else {
              // the previous value NOT was ok, so make edit box blank
              pDE->MakeEmpty();
              pQ->Invalidate();
            }
            pDX->Fail();
          }
        } else {
          // No dependency check values are ok
          pQ->Value(si_val, QU::SI_UNIT);
          pDE->ShowValue(pDE->GetValue());
        }
      } else {
        // the value is not within the boundarys.
        // show boundary messagebox.
        _m()->msg(GetInvalidMaxMinString(sQuantityName, pQ, us), 0, 0);
        if (bWasValid) {
          // the previous value was ok, so resore this value
          pDE->ShowValue(pQ->Value(us));
        } else {
          // the previous value NOT was ok, so make edit box blank
          pDE->MakeEmpty();
          pQ->Invalidate();
        }
        pDX->Fail();
      }
    } else {
      // the edit box is empty
      // invalidate the data.
      pQ->Invalidate();
    }
  } else {
    // From data to controls.
    if (!pQ->Undefined()) {
      // show the value quantity in user units.
      pDE->ShowValue(pQ->Value(us));
    } else {
      // undifined quantities are shown as a
      // blank edit box.
      pDE->MakeEmpty();
    }
  }
}

void AFXAPI DDV_Quantity(CDataExchange *pDX, int nIDC, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us,
                         CString sQuantityName) {

  HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
  CDoubleEdit *pDE = (CDoubleEdit *)pDX->m_pDlgWnd->GetDlgItem(nIDC);

  CQuantityReciever cr(pQ);

  std::string sDependen(sQuantityName);
  if (pDX->m_bSaveAndValidate) {
    // From controls to data

    bool bWasValid = !pQ->Undefined();
    double prevValue;
    if (bWasValid) {
      // restore previous value.
      prevValue = pQ->Value(us);
    }

    if (!pDE->IsEmpty()) {
      // the edit box is not empty
      double si_val = pQ->Convert(pDE->GetValue(), QU::SI_UNIT, us);
      std::string reason;
      if (pQ->CheckValue(si_val, reason)) {
        // the max/min boundarys are ok
        pQ->SetValue(si_val);
        if (!pQ->Undefined()) {
          pDE->ShowValue(pQ->Value(us));
        } else {
          // the previous value NOT was ok, so make edit box blank
          pDE->MakeEmpty();
          pQ->Invalidate();
        }

        if (cr.Refused()) {
          if (!cr.Description().isEmpty()) {
            _m()->msg(cr.Description());
          }
          pDX->Fail();
        }
      } else {
        // the value is not within the boundarys.
        // show boundary messagebox.
        _m()->msg(GetInvalidMaxMinString(sQuantityName, pQ, us), 0, 0);
        if (bWasValid) {
          // the previous value was ok, so resore this value
          pDE->ShowValue(pQ->Value(us));
        } else {
          // the previous value NOT was ok, so make edit box blank
          pDE->MakeEmpty();
          pQ->Invalidate();
        }
        pDX->Fail();
      }
    } else {
      // the edit box is empty
      // invalidate the data.
      pQ->Invalidate();
    }
  } else {
    // From data to controls.
    if (!pQ->Undefined()) {
      // show the value quantity in user units.
      pDE->ShowValue(pQ->Value(us));
    } else {
      // undifined quantities are shown as a
      // blank edit box.
      pDE->MakeEmpty();
    }
  }
}

static CString GetInvalidMaxMinString(CString sQuantityName, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us) {

  CString ret;
  CString frmt;

  if (pQ->MinValue() <= DOUBLE_MIN_) {
    frmt = " -INF";
  } else {
    frmt = CDoubleEdit::Format(pQ->MinValue(us));
  }

  ret = ret + frmt;
  ret = ret + " ";
  ret = ret + pQ->UnitName(us).c_str();
  ret = ret + " ";

  ret = ret + " <";
  if (pQ->IncludeMin()) {
    ret = ret + "=";
  }
  ret = ret + " ";
  ret = ret + sQuantityName;
  ret = ret + " ";

  ret = ret + "<";
  if (pQ->IncludeMax()) {
    ret = ret + "=";
  }
  ret = ret + " ";
  if (pQ->MaxValue() >= DOUBLE_MAX_) {
    frmt = " INF";
  } else {
    frmt = CDoubleEdit::Format(pQ->MaxValue(us));
  }
  ret = ret + frmt;
  ret = ret + " ";
  ret = ret + pQ->UnitName(us).c_str();

  return ret;
}

CString GetUnitString(CDoubleQuantity *pQ, CDoubleQuantity::UNIT us) {

  if (USE_UNITBRACKETS == 0) {
    return CString(pQ->UnitName(us).c_str());
  } else // with unit brackets
  {
    std::stringstream str;
    str << "[" << pQ->UnitName(us).c_str() << "]" << std::ends;
    return CString(str.str().c_str());
  }
}

void SetUnitlabel(CStatic *pUnitLabel, CDoubleQuantity *pQ, CDoubleQuantity::UNIT us) {
  assert(pUnitLabel != 0);
  pUnitLabel->SetWindowText(GetUnitString(pQ, us));
}
