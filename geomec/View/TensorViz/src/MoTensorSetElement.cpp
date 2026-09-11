#include "MoTensorSetElement.h"
#include "stdafx.h"

SO_ELEMENT_SOURCE(MoTensorSetElement);

void MoTensorSetElement::init(SoState *state) {}

/*static*/ void MoTensorSetElement::initClass() { SO_ELEMENT_INIT_CLASS(MoTensorSetElement, SoAccumulatedElement); }

/*static*/ void MoTensorSetElement::add(SoState *state, SoNode *node, const MiTensorSetI *tensorSet) {
  MoTensorSetElement *elt = state->getElement<MoTensorSetElement>();
  elt->m_tensorSets.append((void *)tensorSet);
}

/*static*/ int MoTensorSetElement::getNum(SoState *state) {
  MoTensorSetElement *elt = state->getElement<MoTensorSetElement>();

  return elt->m_tensorSets.getLength();
}

/*static*/ const MiTensorSetI *MoTensorSetElement::get(SoState *state, int index) {
  MoTensorSetElement *elt = state->getElement<MoTensorSetElement>();

  if (index < 0 || index >= elt->m_tensorSets.getLength())
    return 0;

  return (MiTensorSetI *)elt->m_tensorSets.get(index);
}

void MoTensorSetElement::push(SoState *state) {
  MoTensorSetElement *elm = static_cast<MoTensorSetElement *>(getNextInStack());
  m_tensorSets = elm->m_tensorSets;
}
