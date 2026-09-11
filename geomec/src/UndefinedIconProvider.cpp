#include "geomec.h"
#include "resourceIDI.h"
#include "stdafx.h"

#include "UndefinedIconProvider.h"

CUndefinedIconProvider::CUndefinedIconProvider() {}

unsigned int CUndefinedIconProvider::UndefinedIconId() const {
  CGeomecDoc *pDoc = &((CGeomecApp *)AfxGetApp())->GetDoc();
  return (pDoc->ChangeMode() ? IDI_CHANGE_UNDEFINED : IDI_TRI_CHECKED);
}
