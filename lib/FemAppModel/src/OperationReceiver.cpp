// OperationReceiver.cpp: implementation of the COperationReceiver class.
//
//////////////////////////////////////////////////////////////////////
#include "OperationReceiver.h"
#include "FemAppModel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COperationReceiver::COperationReceiver(CFemAppModel &model) : m_bOperation(false) {
  connect(&model, SIGNAL(onBeginOperation()), this, SLOT(slotBeginOperation()));
  connect(&model, SIGNAL(onEndOperation()), this, SLOT(slotEndOperation()));
}

COperationReceiver::~COperationReceiver() {}

void COperationReceiver::slotBeginOperation() {
  m_bOperation = true;
  OnBeginOperation();
}

void COperationReceiver::slotEndOperation() {
  m_bOperation = false;
  OnEndOperation();
}

void COperationReceiver::OnBeginOperation() {}

void COperationReceiver::OnEndOperation() {}

bool COperationReceiver::IsOperation() const { return m_bOperation; }