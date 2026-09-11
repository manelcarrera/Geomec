#include "StdAfx.h"
#include "ZoominModelEntry_Delegate.h"
#include "ZoominModelEntry.h"
#include "HexaModel.h"
#include "GoCadModel.h"
#include "NewModelDlg.h"
#include "ContextMenuInvoker.h"
#include "VersionNumbers.h"
#include "ModelLifetimeFacade.h"
#include "TnoFileDialog.h"
#include "ResultRegister.h"
#include "ConsistencyGuard.h"
#include "GeomecStringTable.h"
#include "resourceIDS.h"
#include "GeomecDoc.h"

CZoominModelEntry_Delegate::CZoominModelEntry_Delegate(CZoominModelEntry* zoominModelEntry)
: CGraphEntryTemp_Delegate<CZoominModelPlaceHolder>(zoominModelEntry),
  m_zoominModelEntry(zoominModelEntry)
{
}

void CZoominModelEntry_Delegate::AppendContextMenu(CContextMenuInvoker& invoker)
{
  typedef CSingleCommandTemplate<CZoominModelEntry_Delegate> TZoominModel_DelegateCommand;

  invoker.AddCommand(QObject::tr("New Zoomin Model").toStdString(),
    *new TZoominModel_DelegateCommand(*this, &CZoominModelEntry_Delegate::NewZoominModel));
  invoker.AddCommand(QObject::tr("Import...").toStdString(),
    *new TZoominModel_DelegateCommand(*this, &CZoominModelEntry_Delegate::Import));
}

void CZoominModelEntry_Delegate::NewZoominModel()
{
  CModelBase& model = static_cast<CModelBase&>(m_zoominModelEntry->Model());

  std::vector<CNewModelDlg::ModelInfo> vcModel;

  CNewModelDlg::ModelInfo modelInfo;

  modelInfo.nType = HEXA_MODEL;
  modelInfo.sType = CHexaModel::staticDocumentType();
  modelInfo.nIcon = CHexaModel::staticModelIconId();
  vcModel.push_back(modelInfo);

  modelInfo.nType = GOCAD_MODEL;
  modelInfo.sType = CTetraModel::staticDocumentType();
  modelInfo.nIcon = CTetraModel::staticModelIconId();
  vcModel.push_back(modelInfo);

  CNewModelDlg dlg(vcModel);

  CModelBase* pChild = 0;

  if(dlg.DoModal() == IDOK)
  {
    int nSelected = dlg.SelectedModel();

    switch (nSelected)
    {
    case HEXA_MODEL:
    case GOCAD_MODEL:
      pChild = IModelLifetimeFacade::NewChild(nSelected, model.Logger(), model.getVersionManager());
    }

    assert(pChild);
    m_zoominModelEntry->AddChildModel(*pChild);
  }
}

void CZoominModelEntry_Delegate::Import()
{
  CModelBase& model = static_cast<CModelBase&>(m_zoominModelEntry->Model());

  CTnoFileDialog dlg(TRUE, GEOMEC_DEFAULT_EXTENSION, 0, OFN_HIDEREADONLY, getStringTableEntry(IDS_FILE_OPEN_FILTER));
  if(dlg.DoModal() == IDOK)
  {
    POSITION pos = dlg.GetStartPosition();
    while(pos)
    {
      CString strFileName = dlg.GetNextPathName(pos);

      CConsistencyGuard *guard = model.GetConsistencyGuard();
      guard->ChildIsLoading(true);

      CModelBase* pChildModel = IModelLifetimeFacade::LoadChild(CModelBase::CModelLoadSaveDefault(&model), LPCSTR(strFileName), model.Logger());

      if(pChildModel)
      {
        // clear stored results from loaded child
        pChildModel->ResultRegister().ClearAll();

        pChildModel->RemoveModelNumbering();

        // transfer data storage items
        for(std::set<unsigned int>::const_iterator it = CModelBase::DataStorageEntryTypes().begin(); it != CModelBase::DataStorageEntryTypes().end(); ++it)
        {
          // get the child model's "real" entry
          CGraphEntry* pChildEntry = pChildModel->CFemAppModel::GraphEntry(*it);

          // get the parent model's entry
          CGraphEntry* pParentEntry = model.GraphEntry(*it);
/*
          if(pChildEntry && pParentEntry)
          {
            CGraphEntry::TEntryNodeSet stNodes = pChildEntry->GraphEntryNodes();
            for(CGraphEntry::TEntryNodeSet::iterator it = stNodes.begin(); it != stNodes.end(); ++it)
            {
              (*it)->reParent(pParentEntry);
              if(pChildEntry->IsLinkedTo(**it))
                pChildEntry->UnLink(**it);
            }
          }
*/
        }
      }

      guard->ChildIsLoading(false);
    }
  }
}
