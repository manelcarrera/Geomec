/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef I_MODEL_FILE_ITEM_H
#define I_MODEL_FILE_ITEM_H

#include "FileMessage.h"
#include <QObject>

#include "ModelFrameExports.h"

class IModelFile;

class MODELFRAME_EXPORT IModelFileItem : public QObject {
  Q_OBJECT

public:
  IModelFileItem(QObject *parent = 0);
  virtual IModelFile &modelFile() = 0;

public:
  void putStatusMessage(const QString &message, const TFilePosition &info = -1);
  void putWarningMessage(int errorNo, const QString &message, const TFilePosition &info = -1);
  void putFatalMessage(int errorNo, const QString &message, const TFilePosition &info = -1);
  void putAbortMessage(int errorNo, const QString &message, const TFilePosition &info = -1);

  virtual operator TFilePosition() = 0;
};

#endif // I_MODEL_FILE_ITEM_H
