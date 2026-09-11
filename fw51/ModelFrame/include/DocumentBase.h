/* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __DOCUMENT_BASE_H
#define __DOCUMENT_BASE_H

#include "IModelObject.h"

#include "ModelFrameExports.h"

#include <QMap>
#include <QString>

class CModelOperationStack;

class MODELFRAME_EXPORT CDocumentBase : public IModelObject {
  Q_OBJECT

  CModelOperationStack *m_operationStack;

  bool m_bOpen;
  QString m_document_description;
  QString m_path_name;

protected:
  virtual void stateChanged(IModelObject &origin);
  virtual void displayChanged(IModelObject &origin);
  virtual void propertyChanged(IModelObject &origin);
  virtual void geometryChanged(IModelObject &origin);
  virtual void parameterChanged(IModelObject &origin);
  virtual void childInserted(IModelObject &origin, IModelObject &child);
  virtual void childRemoved(IModelObject &origin, IModelObject &child);
  virtual void referenceInserted(const IModelObject &origin, const IModelObject &child);
  virtual void referenceRemoved(const IModelObject &origin, const IModelObject &child);
  virtual void error(IModelObject &origin, const QString &description);
  virtual void warning(IModelObject &origin, const QString &description);

  virtual void createDefaults();
  virtual void createContainers() = 0; //!< Client should create empty modelcontainers here.
  void open();

public:
  CDocumentBase(const QString &name = "");
  virtual ~CDocumentBase();

  CModelOperationStack &operationStack();

  const QString &documentDescription() const;
  void documentDescription(const QString &sDescription);

  void setPathName(const QString &name);
  const QString &pathName() const;

  virtual bool canCreateEmptyDocument() const;

  //! Unique type name of document
  virtual QString documentType() const = 0;
  //! Client should return here the current version of the document.
  virtual CStreamVersion documentVersion() const = 0;
  virtual CStreamVersion lastReadableVersion() const;
  virtual bool canReadVersion(const CStreamVersion &version) const;

  bool isOpen();
  bool newDocument();
  virtual bool saveDocument(const QString &sPath, IProgressBase &progress);
  virtual bool loadDocument(const QString &sPath, IProgressBase &progress);
  virtual void closeDocument();

  virtual void restore(TStream &stream, const CStreamVersion &file_version, IProgressBase &indicator);
  virtual void store(TStream &stream, IProgressBase &indicator, bool includeChildren = true) const;

signals:
  void onBeginOperation();
  void onEndOperation();

  void closing();
};

#endif // __DOCUMENT_BASE_H
