#include "MoMeshTensorVector.h"
#include "stdafx.h"

#include <Inventor/engines/SoCalculator.h>
#include <MeshVizXLM/mapping/nodes/MoMeshVector.h>

SO_NODE_SOURCE(MoMeshTensorVector);

void MoMeshTensorVector::initClass() { SO_NODE_INIT_CLASS(MoMeshTensorVector, SoGroup, "Group"); }

void MoMeshTensorVector::exitClass() { SO__NODE_EXIT_CLASS(MoMeshTensorVector); }

MoMeshTensorVector::MoMeshTensorVector() {
  SO_NODE_CONSTRUCTOR(MoMeshTensorVector);

  SO_NODE_ADD_FIELD(scaleFactor, (1.0f));
  SO_NODE_ADD_FIELD(colorScalarSetId, (-1));
  SO_NODE_ADD_FIELD(vectorSetId, (-1));

  m_forwardVector = new MoMeshVector;
  addChild(m_forwardVector);
  m_reverseVector = new MoMeshVector;
  addChild(m_reverseVector);

  m_forwardVector->arrow = false;
  m_forwardVector->scaleFactor.connectFrom(&scaleFactor);
  m_forwardVector->colorScalarSetId.connectFrom(&colorScalarSetId);
  m_forwardVector->vectorSetId.connectFrom(&vectorSetId);

  m_calculator = new SoCalculator;
  m_calculator->expression = "oa = -a;";
  m_calculator->a.connectFrom(&scaleFactor);

  m_reverseVector->arrow = false;
  m_reverseVector->scaleFactor.connectFrom(&m_calculator->oa);
  m_reverseVector->colorScalarSetId.connectFrom(&colorScalarSetId);
  m_reverseVector->vectorSetId.connectFrom(&vectorSetId);
}

MoMeshTensorVector::~MoMeshTensorVector() {}