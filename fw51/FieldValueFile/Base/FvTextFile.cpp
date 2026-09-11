 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
#include "FvTextFile.h"
#include "IProgressBase.h"
#include <qfileinfo.h>
#include <qfile.h>
#include <QTextStream>
#include <cassert>

CTextFile::CTextFile()
: m_unitType( SI_UNIT ),
  m_line_nr(-1),
  m_line_position(-1),
  m_file_position(0),
  m_pProgress(0),
  m_pStream(0)
{
}

CTextFile::~CTextFile()
{
  Clear();
}

const QString& CTextFile::ErrorMessage() const
{
  return m_sErrorMessage;
}

bool CTextFile::Open(const QString &strPath, IProgressBase& progress)
{
  assert(!m_pStream);
  // We open the file and read it...
  m_file.setFileName( strPath );
  if(!m_file.open( QIODevice::ReadOnly | QIODevice::Text )) {
  // Set error message first
  m_sErrorMessage = "Cannot open file";
  return false;
  }

  // Initiprogress
  m_pProgress = &progress;
  m_pProgress->AddSteps(m_file.size());
  m_pProgress->StatusMessage(QString("Loading ") + FileName());

  // Initialise positions
  m_line_nr        = 0;
  m_line_position  = 0;

  m_pStream = new QTextStream(&m_file);

  bool ret(false);

  if(nextLine()) {
  // init dummy
  input_stream stream( *this );

  try {
      ret = OnRead( stream );
  } catch( CReadException error ) {
      // Set error message
     if( lineNr() > 0 ) {
       m_sErrorMessage = QString("Line %1: %2").arg( lineNr() ).arg(error.ErrorMessage());
     } else {
       m_sErrorMessage = error.ErrorMessage();
     } 
     
      ret = false;
  }
  }

  delete m_pStream;
  m_pStream = 0;

  m_file.close();

  return ret;
}


/*!
  This functions is called by the client when an item is saved
*/
void CTextFile::ItemSaved()
{
  assert(m_pProgress);
  m_pProgress->Step();
}

/*!
  This function is called by the client when an item is read
*/
void CTextFile::ItemRead()
{
  qint64 newpos = m_file.pos();
  m_pProgress->Step(newpos - m_file_position);
  m_file_position = newpos;
}


void CTextFile::setUnitType( unitType icode )
{
  m_unitType = icode;
}


/*!
  Returns the items saved in the storage
*/
long CTextFile::SavedItems() const
{
  assert(false);
  // Should be implemented by derived class
  return 0;
}

//##ModelId=3B94A0BE0265
bool CTextFile::Save(const QString &strPath, IProgressBase &progress, bool append )
{
  assert(m_pStream == 0);

  m_pProgress = &progress;

  // Open the file for output
  m_file.setFileName( strPath );
  QIODevice::OpenMode flags = QIODevice::WriteOnly | QIODevice::Text;
  if ( append ) flags |= QIODevice::Append;
  if(!m_file.open( flags ))
  return false;
  m_pStream = new QTextStream( &m_file );

  // Initialise progress callback
  m_pProgress->AddSteps(SavedItems());
  m_pProgress->StatusMessage(QString("Saving ") + FileName());

  // Do write tasks ...
  bool bRet = OnWrite( *m_pStream );

  delete m_pStream;
  m_pStream = 0;

  m_file.close();

  // We should not come to here ...
  return bRet;
}

void CTextFile::Close()
{
  delete m_pStream;
  m_pStream = 0;

  m_file.close();
}

bool CTextFile::Save(FILE* file, IProgressBase &progress)
{
  assert(m_pStream == 0);

  m_pProgress = &progress;

  // Open the file for output
  m_pStream = new QTextStream(file, QIODevice::WriteOnly);

  // Initialise progress callback
  m_pProgress->AddSteps(SavedItems());
  m_pProgress->StatusMessage(QString("Saving ") + FileName());

  // Do write tasks ...
  bool bRet = OnWrite( *m_pStream );

  delete m_pStream;
  m_pStream = 0;

  m_file.close();

  // We should not come to here ...
  return bRet;
}

/*!
  Retrieves next character from the buffer
*/
char CTextFile::get()
{
  assert(m_pStream);

  char ch = peek();

  if( ch == '\n' ) {
  nextLine();
  } else {
  m_line_position++;
  }

  return ch;
}

char CTextFile::peek() const
{
  if( m_line.isNull() ) return 0;
  assert(m_line_position >= 0);
  if(m_line.length() == m_line_position)
  return '\n';
  return m_line.at( m_line_position ).toLatin1();
}

/*!
  Returns current lineNr
*/
int CTextFile::lineNr() const
{
  return m_line_nr;
}

bool CTextFile::nextLine()
{
  // Retrieves the next line
  m_line = m_pStream->readLine();
  if(m_line.isNull()) {
  // End of file ...
  m_line_position = -1;
  return false;
  }
  m_line_nr++;
  
  if(m_line.length() == 0) return nextLine();

  // locate literal names using quotes
  typedef std::pair<int, int> TQuotePair;
  typedef std::vector<TQuotePair> TQuoteVec;

  TQuoteVec vcQuotes;

  // this code doesn't bother about unmatched quotes, this is dealt with later in the parsing code
  int nQuotePos = m_line.indexOf('"');
  bool bFirst = true;
  TQuotePair prCurrent;
  while(nQuotePos != -1)
  {
    if(bFirst)
    {
      prCurrent.first = nQuotePos;
    }
    else
    {
      prCurrent.second = nQuotePos;
      vcQuotes.push_back(prCurrent);
    }

    nQuotePos = m_line.indexOf('"', nQuotePos + 1);
    bFirst = !bFirst;
  }

  // Take out comment
  for(std::set<QString>::iterator it = m_stComment.begin(); it != m_stComment.end(); it++)
  {
     int nPosition = m_line.indexOf(*it);
     if(nPosition == 0) {
       return nextLine();
     }
     if(nPosition > 0) {
     bool bIsAComment = true;
     // don't take out if the position is inside any of the quoted sections
     for(size_t i = 0; i < vcQuotes.size(); ++i)
     {
       if(nPosition > vcQuotes[i].first && nPosition < vcQuotes[i].second)
       {
         // inside quoted section, not an actual comment
         bIsAComment = false;
         break;
       }
     }

     if(bIsAComment)
     {
      m_line = m_line.left(nPosition);
      m_line_position = 0;
      return true;
     }
     }
  }

  m_line_position = 0;

  return true;
}
  
/*!
  returns the full path of the file
*/
QString CTextFile::PathName() const
{
  return m_file.fileName();
}

/*!
  Returns the file name only
*/
QString CTextFile::FileName() const
{
  QFileInfo file_info( PathName() );
  return file_info.fileName();
}

/*!
  Just parsing the file
*/
bool CTextFile::OnRead(TInputStream& stream)
{
  // Defaultly we will start parsing the file
  return Parse( stream );
}


//##ModelId=3B94A0BE0246
bool CTextFile::OnWrite(TOutputStream& /*stream*/)
{
  // Implementation
  assert(false);

  return false;
}

bool CTextFile::OnParseFail(TInputStream& /*stream*/, const QString& sToken)
{
  m_sErrorMessage = QString("Unknown keyword %1").arg(sToken);
  return false;
}

void CTextFile::OnParseSucceed(TInputStream& /*stream*/, const QString& /*sToken*/)
{
}

//##ModelId=3B94A0BD0352
bool CTextFile::DefineComment(const QString &sTag)
{
  assert(sTag.length() > 0);
  return m_stComment.insert(sTag).second;
}

bool CTextFile::UndefineComment(const QString &sTag)
{
   assert(sTag.length() > 0);
  std::set<QString>::iterator it = m_stComment.find(sTag);
  if(it == m_stComment.end()) return false;

  m_stComment.erase(it);

  return true;
}

/*!
  Defines a token and a reading routine. Token cannot contain spaces
*/
bool CTextFile::DefineToken(CToken &token, bool bAutoDeleteToken)
{
  assert(token.Token().length() > 0);
  assert(!token.Token().contains(' ')); 

  // Try to insert ...
  if(m_mpTokenMap.insert(token_map::value_type(token.Token().toUpper(), token_type(&token, bAutoDeleteToken))).second)
  return true;

  // The definition of this token failed so return false and depending on bAutoDeleteToken delete the token
  if (bAutoDeleteToken)
  delete &token;

  return false;
}

void CTextFile::Clear()
{
  // Iterate token map and delete token pointer if auto delete is enabled
  for(token_map::iterator iter = m_mpTokenMap.begin(); iter != m_mpTokenMap.end(); iter++)
  {
  if(iter->second.second)      // Extract auto delete tag
  {
      delete iter->second.first;  // Extract token map ptr
  }
  }

  m_mpTokenMap.clear();
  m_stComment.clear();
}

bool CTextFile::Parse( TInputStream& stream )
{
  stream.eatwhite();
  while( !stream.eof() ) {
  QString sToken; // Token we're evaluating ...
  stream >> sToken;

  // jbj
  // Mantis 2289
  // if sToken == "AXIS_UNIT" we cannot process until we read the next 3 tokens:
  // "ft" "ft" "ft"
  // or
  // "m" "m" "m"
  //
  // For now we will apply a quick patch so we can read "ft" input, by using
  // this.setUnitType, and restoring to SI_UNIT afterwards.  This is NOT an
  // optimal solution since file formats other than GoCad are now also 
  // receiving "ft" tokens, but it is the least intrusive solution for now.
  //
  // TODO
  // The ->Read indirection and the tokenmap are inadequate here.
  // Maintenance--.
  // There will be more new tokens in the future and the problems with the
  // current architecture will only get worse.  Proposed solution:
  // Remove tokenmap, remove CToken class c.s., and replace it by a much
  // simpler design with a proper use of polymorphism.
  //
  
  if ( sToken == QString("\"ft\"") )
    setUnitType( FIELD_UNIT ); // temporarily: we are reading "ft"
                               // but we will store in "m" (SI_UNIT)
  
  // Look up the token in the tokenmap
  if(m_mpTokenMap.find(sToken.toUpper()) != m_mpTokenMap.end()) {
      try {
    if(m_mpTokenMap.find(sToken.toUpper())->second.first->Read(stream, sToken)) {
          OnParseSucceed(stream, sToken);   // Go on parsing
          stream.eatwhite();
    } else {
          OnParseSucceed(stream, sToken);   // Stop parsing
          return true;
    }
      } catch( CReadException error ) {
    // Set error message
    if( lineNr() > 0 ) {
          m_sErrorMessage = QString("Line %1: %2").arg( lineNr() ).arg(error.ErrorMessage());
    } else {
          m_sErrorMessage = error.ErrorMessage();
    }
    return false; // Failed
      }
  } else {
      // Parsing failure
      if( OnParseFail( stream, sToken ) ) {
    stream.eatwhite(); // Go on parsing
      } else {
    return false;      // Stop parsing
      }
  }
  }
  setUnitType( SI_UNIT ); // Restore: data stored as "m", not "ft"

  // End of file
  return true;
}


