 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#ifndef __MODEL_FILE_OBS_H
#define __MODEL_FILE_OBS_H

#include <QObject>

#include "GuiFrameExports.h"

class QTextStream;
class IModelFile;
struct TFileMessage;
struct TFileError;
struct TModelError;
struct TFilePosition;

class GUIFRAME_EXPORT CModelFileObs : public QObject
{
  Q_OBJECT

public:
  CModelFileObs( IModelFile& modelFile, bool bSuppressStatus = true );
  CModelFileObs( IModelFile& modelFile,
                 QTextStream* outStream,
                 QTextStream* errStream,
                 bool bSuppressStatus = true );
  virtual ~CModelFileObs();

private slots:
  void onStatusMessage(  const TFileMessage& msg );
  void onWarningMessage( const TFileError&   msg );
  void onFatalMessage(   const TFileError&   msg );
  void onAbortMessage(   const TFileError&   msg );

private:
  void init( IModelFile& modelFile );
  void putMessage( QTextStream&, const QString& severity, const TModelError& msg );
  void putLocation( QTextStream&, const TFilePosition& info );
  QTextStream* m_outStream;
  bool m_weOwnOutStream;
  QTextStream* m_errStream;
  bool m_weOwnErrStream;
  bool m_suppressStatus;
};

#endif // __MODEL_FILE_OBS_H
