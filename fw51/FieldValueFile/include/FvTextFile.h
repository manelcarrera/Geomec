 /* Copyright (c) 2011 TNO DIANA BV                              Confidential */
// TextFile.h: interface for the CTextFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTFILE_H__F3DCB857_4132_42DF_AD54_E74F8014368B__INCLUDED_)
#define AFX_TEXTFILE_H__F3DCB857_4132_42DF_AD54_E74F8014368B__INCLUDED_

#include <set>
#include <map>
#include <vector>
#include <QTextStream>
#include "FvInputStream.h"
#include <qstring.h>
#include <qfile.h>
#include "FvToken.h"

#include "FieldValueFileExports.h"

enum unitType { FIELD_UNIT, SI_UNIT, OTHER_UNIT };

class IProgressBase;
class FIELDVALUEFILE_EXPORT CTextFile 
{
public:
  friend class input_stream;
  typedef input_stream TInputStream;
  typedef QTextStream  TOutputStream;
  
  int lineNr() const;
    
    // Construction / destruction
    CTextFile();
    virtual ~CTextFile();

    // End - client interface ...
    bool Open(const QString &strPath, IProgressBase &progress);
    
    bool Save(const QString &strPath, IProgressBase &progress, bool append = false);
    bool Save(FILE* file, IProgressBase &progress);

    void Close();

    void ItemSaved();
    void ItemRead();

	virtual void setUnitType( unitType );

    // Path and file name access
    QString PathName() const;
    QString FileName() const;

    const QString& ErrorMessage() const;

    // Clear the token and comments stored in the parser. Always called by the constructor
    virtual void Clear();

    // Function to start the parser
    bool Parse( TInputStream& stream );

    // Functions to define a token or comment in the parser
    virtual bool DefineToken(CToken &token, bool bAutoDeleteToken = true);

    // Functions to read a line in one string or every word in a seperate string
//    std::vector<QString> ReadLineSegmented();
//    QString ReadLine();
protected:
  virtual void OnParseSucceed(TInputStream& stream, const QString& sToken);
  virtual bool OnParseFail(TInputStream& stream, const QString &sToken);

  // Derived interface ...
  virtual bool OnRead( TInputStream& stream );
  virtual bool OnWrite( TOutputStream& stream );
  virtual long SavedItems() const;

  virtual bool DefineComment(const QString &sTag);
  virtual bool UndefineComment(const QString &sTag);
  unitType   m_unitType;

private:
  // The token parser has a token map
  typedef std::pair<CToken*, bool> token_type;
  // Map where the tokens are stored
  typedef std::map<QString, token_type> token_map;

  token_map         m_mpTokenMap;
  QString           m_line;
  int               m_line_nr;
  int               m_line_position;
  quint64           m_file_position;
  QFile             m_file;
  IProgressBase*    m_pProgress;
  QString           m_sErrorMessage;  // Error message when parser fails
  std::set<QString> m_stComment;
  QTextStream*      m_pStream;

  bool nextLine();
  const QString& currentLine() const;

  char get();
  char peek() const;
};

#endif // !defined(AFX_TEXTFILE_H__F3DCB857_4132_42DF_AD54_E74F8014368B__INCLUDED_)
