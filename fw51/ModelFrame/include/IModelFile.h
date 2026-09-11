 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __I_MODEL_FILE_H
#define __I_MODEL_FILE_H

#include <QObject>
#include "FileMessage.h"

#include "ModelFrameExports.h"

class QTextStream;

struct MODELFRAME_EXPORT TFatalException : public TFileError
{
  TFatalException( const TFileError& msg ) : TFileError( msg ) {}
};

struct MODELFRAME_EXPORT TAbortException : public TFileError
{
  TAbortException( const TFileError& msg ) : TFileError( msg ) {}
};

struct MODELFRAME_EXPORT DianaAssertMessage {
  QString      assertion;
  QString      file;
  unsigned int line;
  DianaAssertMessage( const char*  __expr,
                      const char*  __file,
                      unsigned int __line ) {
  assertion = __expr;
  file      = __file;
  line      = __line;
  }
};

class IModelObject;
class IModelFileItem;

class MODELFRAME_EXPORT  IModelFile : public QObject
{
  Q_OBJECT

  friend class IModelFileItem;

public:
  virtual bool canRead( const QString& fileName ) const = 0;
  virtual QString formatExtension() const = 0;
  virtual QString formatDescription() const = 0;

  bool readModel(  IModelObject& root, const QString& fileName );
  bool readModel(  IModelObject& root, FILE* file );
  bool writeModel( IModelObject& root, const QString& fileName );
  bool writeModel( IModelObject& root, FILE* file );
  bool writeModel( IModelObject& root, QTextStream& stream );
  int  setMaxFatal( int nFatal );

  IModelObject& rootObject();
  const IModelObject& rootObject() const;

private:
  void assertHandler( const DianaAssertMessage& msg );

signals:
  void statusMessage(  const TFileMessage& message );
  void warningMessage( const TFileError&   message );
  void fatalMessage(   const TFileError&   message );
  void abortMessage(   const TFileError&   message );
  void progress( int step, int total );
  void done();

public slots:
  void onCancel();

protected:
  IModelFile();

  virtual void init() = 0;
  virtual void clear() = 0;
  virtual bool read(  const QString& fileName ) = 0;
  virtual bool read(  FILE* file ) = 0;
  virtual bool write( const QString& fileName );
  virtual bool write( FILE* file );
  virtual bool write( QTextStream& stream );

  virtual void putStatusMessage(  const TFileMessage& message );
  virtual void putWarningMessage( const TFileError&   message );
  virtual void putFatalMessage(   const TFileError&   message );
  virtual void putAbortMessage(   const TFileError&   message );
  void putProgress( int step, int total );
  void checkCancel();

protected:
  bool m_succes;

private:
  IModelObject* m_root;
  bool          m_cancel;
  int           m_nFatal;
  int           m_maxFatal;
};

#endif // __I_MODEL_FILE_H
