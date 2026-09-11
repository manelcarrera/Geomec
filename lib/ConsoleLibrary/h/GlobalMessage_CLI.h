#pragma once

// From c:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\WinUser.h
#define MB_OK 0x00000000L
#define MB_OKCANCEL 0x00000001L
#define MB_YESNO 0x00000004L

#define MB_ICONHAND 0x00000010L
#define MB_ICONQUESTION 0x00000020L
#define MB_ICONEXCLAMATION 0x00000030L
#define MB_ICONASTERISK 0x00000040L

#define MB_ICONERROR MB_ICONHAND
#define MB_ICONINFORMATION MB_ICONASTERISK

#define IDOK 1
#define IDCANCEL 2
#define IDYES 6
#define IDNO 7

#include <vector>
//#include <iostream>

#include <fstream>

#include <QString>

#include "IGlobalMessage.h"

class GlobalMessage_CLI : virtual public IGlobalMessage
{
  std::fstream out_stream;

  
public:
  std::ostream& outstream();

  bool openlogfile( const std::string& path );
  bool closelogfile();

public:
  GlobalMessage_CLI(){ m_type = CLI; };

  int msg(	
    const char* message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual std::string constructMessage(unsigned int message);

  void status( const QString& message );
};





#ifdef KK

std::ostream& outstream();
bool openlogfile(const std::string& path);
bool closelogfile();

std::string constructMessage(const std::vector <std::string>& message);

int _m()->msg(	const QString& message, 
          unsigned int style = MB_OK,
          unsigned int contextID = 0);

int _m()->msg(	const char* message, 
          unsigned int style = MB_OK,
          unsigned int contextID = 0);

int _m()->msg(	unsigned int message, 
          unsigned int style = MB_OK,
          unsigned int contextID = 0);

int _m()->msg(	const std::vector <std::string>& message,
          unsigned int style = MB_OK, 
          unsigned int contextID = 0);

int globalTetMesherMessage(	const std::vector <std::string>& message,
              unsigned int style = MB_OK, 
              unsigned int contextID = 0);

void statusMessage(const QString& message);

#endif