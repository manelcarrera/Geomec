/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_LOCATION_H
#define __MODEL_LOCATION_H

#include "IModelObject.h"
#include <QStack>

#include "ModelFrameExports.h"

class IModelObject;
class CDocumentBase;

class MODELFRAME_EXPORT CModelLocation {
public:
  CModelLocation();
  CModelLocation(const IModelObject &object);
  virtual ~CModelLocation();

  IModelObject *getObject(CDocumentBase &document);

  typedef IModelObject::TStream TStream;
  void store(TStream &stream) const;
  void restore(TStream &stream, const CStreamVersion &streamVersion);

private:
  QStack<int> m_positions;
};

#endif // __MODEL_LOCATION_H
