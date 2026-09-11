/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "TestHorizon.h"
#include "TestIModelObjectReceiver.h"

namespace test_lib {

CHorizon::CHorizon(QTextStream &os) : IModelObject() {
  new CModelObjectReceiver(*this, os); // Receiver is automatically destroyed
}

CHorizon::CHorizon(const QString &sName, QTextStream &os) : IModelObject(sName) {
  new CModelObjectReceiver(*this, os); // Receiver is automatically destroyed
}

CHorizon::~CHorizon() {}

CHorizonContainer::CHorizonContainer(QTextStream &os) : CModelContainer<CHorizon>() {
  new CModelObjectReceiver(*this, os);
}

CHorizonContainer::CHorizonContainer(const QString &text, QTextStream &os) : CModelContainer<CHorizon>(text) {
  new CModelObjectReceiver(*this, os);
}

} // namespace test_lib