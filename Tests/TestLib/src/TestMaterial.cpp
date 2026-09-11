/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// Material.cpp: implementation of the CMaterial class.
//
//////////////////////////////////////////////////////////////////////
#include "TestMaterial.h"
#include "TestIModelObjectReceiver.h"

namespace test_lib {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaterial::CMaterial(QTextStream &os) : IModelObject() { new CModelObjectReceiver(*this, os); }

CMaterial::CMaterial(const QString &sName, QTextStream &os) : IModelObject(sName) {
  new CModelObjectReceiver(*this, os);
}

CMaterial::~CMaterial() {}

CMaterialContainer::CMaterialContainer(QTextStream &os) : CModelContainer<CMaterial>() {
  new CModelObjectReceiver(*this, os);
}

CMaterialContainer::CMaterialContainer(const QString &text, QTextStream &os) : CModelContainer<CMaterial>(text) {
  new CModelObjectReceiver(*this, os);
}

} // namespace test_lib