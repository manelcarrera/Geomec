#ifndef MOMESHTENSORVECTOR_H_INCLUDED
#define MOMESHTENSORVECTOR_H_INCLUDED

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>

class MoMeshVector;
class SoCalculator;

class MoMeshTensorVector : public SoGroup
{
  SO_NODE_HEADER(MoMeshTensorVector);

  MoMeshVector* m_forwardVector;
  MoMeshVector* m_reverseVector;
  SoCalculator* m_calculator;

public:

  static void initClass();
  static void exitClass();

  SoSFFloat scaleFactor;

  SoSFInt32 colorScalarSetId;

  SoSFInt32 vectorSetId;

  MoMeshTensorVector();

  virtual ~MoMeshTensorVector();
};

#endif
