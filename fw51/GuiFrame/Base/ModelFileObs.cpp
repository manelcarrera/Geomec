 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "ModelFileObs.h"
#include "IModelFile.h"
#include <QTextStream>

/*!
  \class CModelFileObs
  \brief Redirects messages from IModelFile instances to stderr and stdout.
*/

/*!
  Constructs an instance and connects all IModelFile signals to the
  appropriate slots.
  \param suppressStatus Suppress status messages when true
*/
CModelFileObs::CModelFileObs( IModelFile& modelFile, bool suppressStatus )
: m_outStream( NULL ),
  m_weOwnOutStream( false ),
  m_errStream( NULL ),
  m_weOwnErrStream( false ),
  m_suppressStatus( suppressStatus )
{
  init( modelFile );
}

/*!
  Constructor for when other text streams may be operating on stdout/stderr
  so we need to use them to make sure things stay in order.

  Will create (and delete) its own streams if they are NULL, otherwise does
  not own them.

  QT4_REVIEW maybe make this the only constructor?
*/
CModelFileObs::CModelFileObs( IModelFile& modelFile,
                              QTextStream* outStream,
                              QTextStream* errStream,
                              bool suppressStatus )
: m_outStream( outStream ),
  m_weOwnOutStream( false ),
  m_errStream( errStream ),
  m_weOwnErrStream( false ),
  m_suppressStatus( suppressStatus )
{
  init( modelFile );
}

CModelFileObs::~CModelFileObs()
{
  if ( m_weOwnOutStream ) delete m_outStream;
  if ( m_weOwnErrStream ) delete m_errStream;
}

/*!
  Slot to redirect status messages to stdout.
*/
void CModelFileObs::onStatusMessage( const TFileMessage& message )
{
  if(!m_suppressStatus) {
    *m_outStream << message.text << endl;
    putLocation( *m_outStream, message );
  }
}

/*!
  Slot to redirect warning messages to stderr.
*/
void CModelFileObs::onWarningMessage( const TFileError& message )
{
  putMessage( *m_errStream, tr( "Warning" ), message );
  putLocation( *m_errStream, message );
}

/*!
  Slot to redirect fatal messages to stderr.
*/
void CModelFileObs::onFatalMessage( const TFileError& message )
{
  putMessage( *m_errStream, tr( "Fatal" ), message );
  putLocation( *m_errStream, message );
}

/*!
  Slot to redirect abort messages to stderr.
*/
void CModelFileObs::onAbortMessage( const TFileError& message )
{
  putMessage( *m_errStream, tr( "Abort" ), message );
  putLocation( *m_errStream, message );
}

void CModelFileObs::init( IModelFile& modelFile )
{
  if ( !m_outStream )
  {
    m_outStream = new QTextStream( stdout, QIODevice::WriteOnly );
    m_weOwnOutStream = true;
  }
  if ( !m_errStream )
  {
    m_errStream = new QTextStream( stderr, QIODevice::WriteOnly );
    m_weOwnErrStream = true;
  }

  connect( &modelFile, SIGNAL( statusMessage(    const TFileMessage&   ) ),
           this,       SLOT(   onStatusMessage(  const TFileMessage&   ) ) );
  connect( &modelFile, SIGNAL( warningMessage(   const TFileError&     ) ),
           this,       SLOT(   onWarningMessage( const TFileError&     ) ) );
  connect( &modelFile, SIGNAL( fatalMessage(     const TFileError&     ) ),
           this,       SLOT(   onFatalMessage(   const TFileError&     ) ) );
  connect( &modelFile, SIGNAL( abortMessage(     const TFileError&     ) ),
           this,       SLOT(   onAbortMessage(   const TFileError&     ) ) );
}

/*!
  Formats and writes an error message.
  \param fp A file pointer like stdout or stderr.
  \param severity A string representation like "Fatal" or "Warning".
  \param msg The message contents.
*/
void CModelFileObs::putMessage( QTextStream& os, const QString& severity, const TModelError& msg )
{
  os << tr( "Error code: /diana/lb/fw/" ) << msg.error << endl;
  os << tr( "Severity  : " )              << severity  << endl;          
  if ( !msg.url.isEmpty()  ) os << tr( "Reference : " ) << msg.url  << endl;
  if ( !msg.text.isEmpty() ) os << tr( "Message   : " ) << msg.text << endl;          
}

/*!
  Formats and writes location information.
  \param fp A file pointer like stdout or stderr.
  \param info The location information.
*/
void CModelFileObs::putLocation( QTextStream& os, const TFilePosition& info )
{
  if ( info.lineNr > -1 ) {
    QString prefix( tr( "Line %1: " ).arg( info.lineNr ) );
    os << prefix << info.line;
    if ( !info.line.endsWith( "\n" ) ) os << endl;

    if ( info.pos > -1 ) {
      QString pointer;
      pointer.fill( ' ', prefix.length() + info.pos );
      os << pointer << '^' << endl;
    }
  }
}
