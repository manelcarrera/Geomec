/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_MODEL_OBJECT_FACTORY_H
#define __I_MODEL_OBJECT_FACTORY_H

#include "ModelObjectFactoryExports.h"

class IModelObject;

class MODELOBJFACTORY_EXPORT IModelObjectFactory {
public:
  static IModelObject *create(const char *className);
  virtual IModelObject *createObject(const char *className) const = 0;

protected:
  IModelObjectFactory();
  virtual ~IModelObjectFactory();
};

#endif // __I_MODEL_OBJECT_FACTORY_H
