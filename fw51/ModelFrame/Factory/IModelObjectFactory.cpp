/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "IModelObjectFactory.h"
#include "IModelObject.h"
#include "ModelMessage.h"
#include <QList>
#include <cassert>

/*!
 The singleton collection class for IModelObjectFactory
 */
class IModelObjectFactoryCollection {
public:
  static IModelObjectFactoryCollection &instance();

  void insert(IModelObjectFactory *factory);
  void remove(IModelObjectFactory *factory);
  IModelObject *create(const char *className);

protected:
  IModelObjectFactoryCollection();
  virtual ~IModelObjectFactoryCollection();

private:
  QList<IModelObjectFactory *> m_factories;
  static IModelObjectFactoryCollection *m_instance;
};

// static
IModelObjectFactoryCollection *IModelObjectFactoryCollection::m_instance = 0;

/*!
  The private constructor, may only be called via instance
 */
IModelObjectFactoryCollection::IModelObjectFactoryCollection() { assert(m_instance == 0); }

IModelObjectFactoryCollection::~IModelObjectFactoryCollection() { m_instance = 0; }

/*!
  Returns the one and only instance of of IModelObjectFactoryCollection;
 */
IModelObjectFactoryCollection &IModelObjectFactoryCollection::instance() {
  if (m_instance == 0)
    m_instance = new IModelObjectFactoryCollection();
  return *m_instance;
}

/*!
  Insert the factory in the factory collection
 */
void IModelObjectFactoryCollection::insert(IModelObjectFactory *factory) { m_factories.append(factory); }

/*!
  Remove the factory from the factory collection
 */
void IModelObjectFactoryCollection::remove(IModelObjectFactory *factory) { m_factories.removeOne(factory); }

IModelObject *IModelObjectFactoryCollection::create(const char *className) {
  for (QList<IModelObjectFactory *>::const_iterator it = m_factories.begin(); it != m_factories.end(); ++it) {
    IModelObjectFactory *f = *it;
    IModelObject *o = f->createObject(className);
    if (o)
      return o;
  }
  throw TModelError(QObject::tr("Class '%1' not implemented in any of the "
                                " registered factories")
                        .arg(className),
                    7030);
}

namespace {

const QString CLASS_NAME = QObject::tr("IModelObject");

} // anonymous namespace

IModelObject *IModelObjectFactory::create(const char *className) {
  DIA_ASSERT(className != CLASS_NAME);
  return IModelObjectFactoryCollection::instance().create(className);
}

IModelObjectFactory::IModelObjectFactory() { IModelObjectFactoryCollection::instance().insert(this); }

IModelObjectFactory::~IModelObjectFactory() { IModelObjectFactoryCollection::instance().remove(this); }
