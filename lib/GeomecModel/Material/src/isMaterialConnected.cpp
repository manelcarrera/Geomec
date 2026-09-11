#include "isMaterialConnected.h"
#include "FormationBase.h"

bool isMaterialConnected(CModelBase *modelBase, IMaterialBase* material)
{
  TFormationBaseEntry* formationBaseEntry =
  dynamic_cast <TFormationBaseEntry*> (
      modelBase->GraphEntry(MD_BASE_FORMATION));
  CDepletionStageEntry* depletionStageEntry =
  dynamic_cast <CDepletionStageEntry*> (
      modelBase->GraphEntry(MD_BASE_DEPLETION_STAGE));
  bool connected = false;

  for (TFormationBaseEntry::TNodeSet::const_iterator formation =
  formationBaseEntry->EntryNodes().begin();
  !connected && (formation != formationBaseEntry->EntryNodes().end());
  ++formation)
  {
  for (CDepletionStageEntry::iterator stage = depletionStageEntry->begin();
      !connected && (stage != depletionStageEntry->end()); ++stage)
  {
      if ((*formation)->ConnectedMaterial(*stage) != 0)
      {
    connected = connected ||
          (*formation)->ConnectedMaterial(*stage)->IsLinkedTo(*material);
      }
  }
  }

  return connected;
}
