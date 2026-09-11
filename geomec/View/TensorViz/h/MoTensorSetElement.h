#ifndef MOTENSORSETELEMENT_H_INCLUDED
#define MOTENSORSETELEMENT_H_INCLUDED

#include "MoTensorSet.h"

#include <Inventor/SbPList.h>
#include <Inventor/elements/SoAccumulatedElement.h>

class MoTensorSetElement : public SoAccumulatedElement {
  SO_ELEMENT_HEADER(MoTensorSetElement);

  SbPList m_tensorSets;

  virtual void init(SoState *state);

public:
  static void initClass();

  static void add(SoState *state, SoNode *node, const MiTensorSetI *tensorSet);

  static int getNum(SoState *state);

  static const MiTensorSetI *get(SoState *state, int index);

  virtual void push(SoState *state);
};

#endif
