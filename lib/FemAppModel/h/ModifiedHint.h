#ifndef _ModifiedHint_h_
#define _ModifiedHint_h_

enum ModifiedHint
{
  Default = 0,
  MeshCleared = 1,
  MeshCreated = 2,
  MapTypeChanged = 3,
  SelectionChanged = 4,
  MarkedAsInitialChanged = 5, // was 1 in CDepletionStage
  MATERIAL_ATTACHED = 6, // was 320
  MATERIAL_REMOVED = 7, // was 330
  GEOMETRY_CHANGED = 8, // was 1 
  PD_POSCHANGED = 9, // was 1
  NEW_FORMATION_ATTACHED = 10, // was 300
  FORMATION_DELETED = 11, // was 310
  MESH_LOADED = 12, // was 350
  MAT_PARAM_CHANGED = 13, // was 244
  WELLPATH_FLIPPED = 14,
  COLOR_CHANGED = 15,
  NAME_CHANGED = 16
};

#endif  // _ModifiedHint_h_
