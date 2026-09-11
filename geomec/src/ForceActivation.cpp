
#include "stdafx.h"

#include "GraphPtr_Delegate.h"
#include "OpenGLSceneBase.h"
#include "ValueTensor_Delegate.h"
#include "ValueVector_Delegate.h"

/*
 * Force the instantiation of template delegate types that
 * are not activated by the use of the macro ACTIVATE_TEMPLATE_DELEGATE.
 * See also the comment in the file Delegate.h.
 */

void forceActivation(CGraphPtr<CGraphNode> &graphPtr) { CGraphPtr_Delegate<CGraphNode> graphPtr_Delegate(&graphPtr); }

void forceActivation(CGraphPtr<COpenGLSceneBase> &graphPtr) {
  CGraphPtr_Delegate<COpenGLSceneBase> graphPtr_Delegate(&graphPtr);
}

void forceActivation(CGraphPtr<COpenGLSceneNode> &graphPtr) {
  CGraphPtr_Delegate<COpenGLSceneNode> graphPtr_Delegate(&graphPtr);
}

void forceActivation(CGraphPtr<IValueComponentBase> &graphPtr) {
  CGraphPtr_Delegate<IValueComponentBase> graphPtr_Delegate(&graphPtr);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_XX, 0> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_XX, 0> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_YY, 1> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_YY, 1> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_ZZ, 2> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_ZZ, 2> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_XY, 3> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_XY, 3> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_YZ, 4> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_YZ, 4> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CTensorComponentTemp<IDS_COMPONENT_XZ, 5> &tensorComponentTemp) {
  CTensorComponentTemp_Delegate<IDS_COMPONENT_XZ, 5> tensorComponentTemp_Delegate(&tensorComponentTemp);
}

void forceActivation(CVectorComponentTemp<IDS_COMPONENT_XX, 0> &vectorComponentTemp) {
  CVectorComponentTemp_Delegate<IDS_COMPONENT_XX, 0> vectorComponentTemp_Delegate(&vectorComponentTemp);
}

void forceActivation(CVectorComponentTemp<IDS_COMPONENT_YY, 1> &vectorComponentTemp) {
  CVectorComponentTemp_Delegate<IDS_COMPONENT_YY, 1> vectorComponentTemp_Delegate(&vectorComponentTemp);
}

void forceActivation(CVectorComponentTemp<IDS_COMPONENT_ZZ, 2> &vectorComponentTemp) {
  CVectorComponentTemp_Delegate<IDS_COMPONENT_ZZ, 2> vectorComponentTemp_Delegate(&vectorComponentTemp);
}
