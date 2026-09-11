#include "MoTensorSet.h"
#include "MoTensorSetElement.h"
#include "stdafx.h"

SO_NODE_SOURCE(MoTensorSet);

/*static*/ void MoTensorSet::initClass() { SO_NODE_INIT_CLASS(MoTensorSet, MoActionNode, "MoActionNode"); }

/*static*/ void MoTensorSet::exitClass() { SO__NODE_EXIT_CLASS(MoTensorSet); }

void MoTensorSet::doAction(SoAction *action) {}

void MoTensorSet::GLRender(SoGLRenderAction *action) {
  SoState *state = action->getState();

  MoTensorSetElement::add(state, this, m_tensorSet);
}

MoTensorSet::MoTensorSet() : m_tensorSet(0) { SO_NODE_CONSTRUCTOR(MoTensorSet); }

void MoTensorSet::setTensorSet(const MiTensorSetI *tensorSet) {
  m_tensorSet = tensorSet;
  touch();
}

const MiTensorSetI *MoTensorSet::getTensorSet() { return m_tensorSet; }

void MoTensorSet::reset() { setTensorSet(0); }
