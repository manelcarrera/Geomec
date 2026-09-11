// GocadExportWrapper.cpp: implementation of the CGocadExportWrapper class.
//
//////////////////////////////////////////////////////////////////////
#include "Geomec.h"
#include "StdAfx.h"

#include "ExportGocadDlg.h"
#include "GlobalMessage.h"
#include "GocadExportWrapper.h"
#include "TetraModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGocadExportWrapper::CGocadExportWrapper() {}

void CGocadExportWrapper::Export(CModelBase &model, const CResultGroup *pResultGroup) {
  if (!dynamic_cast<const CTetraModel *>(&model)) {
    _m()->msg("Gocad export is currently only possible for tetrahedron models");
    return;
  }

  CGocadExport ge(model);
  CExportGocadDlg dlg(ge, pResultGroup);
  dlg.DoModal();
}
