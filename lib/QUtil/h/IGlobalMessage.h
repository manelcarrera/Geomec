#pragma once

#include <vector>

#include <QString>

// FIXME
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

#include <iostream>

class IGlobalMessage
{
public:

  class IGlobalMessageObserver
  {
  public:
  IGlobalMessageObserver();
  virtual ~IGlobalMessageObserver();

  virtual void notifyWarning(const std::string& message);
  virtual void notifyError(const std::string& message);
  };

  enum eType{ Empty, GUI, GUI_MFC, CLI, Undefined };

protected:

  eType m_type;
  void type(eType val){m_type=val;}

  virtual void notifyWarning(const std::string& message);
  virtual void notifyError(const std::string& message);

public:

  eType type(){return m_type;}

  void AttachObserver(IGlobalMessageObserver *observer);
  void DetachObserver(IGlobalMessageObserver *observer);

  // TODO: the int message versions for GUI need to be checked whether all the resource strings
  //       are captured in the GeomecStringTable; if so, we can replace it
  //       For now, the GUI still provides its own version, and warn/error might not work well there.

  virtual int msg(	
    const QString& message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual int msg(	
    const char* message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0) = 0;

  virtual int msg(	
    unsigned int message, 
    unsigned int style = MB_OK,
    unsigned int contextID = 0);

  virtual int msg(	
    const std::vector <std::string>& message,
    unsigned int style = MB_OK, 
    unsigned int contextID = 0);

  virtual int warn(
  const QString& message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int warn(
  const char* message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int warn(
  unsigned int message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int warn(
  const std::vector <std::string>& message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int error(
  const QString& message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int error(
  const char* message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int error(
  unsigned int message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual int error(
  const std::vector <std::string>& message,
  unsigned int style = MB_OK,
  unsigned int contextID = 0);

  virtual void status( const QString& message ) = 0;

  IGlobalMessage(){m_type=Undefined;};
  virtual ~IGlobalMessage(){};

  // Just for CLI
  virtual std::ostream& outstream(){ return std::cout; };
  virtual bool openlogfile( const std::string& path ){ return false; };
  virtual bool closelogfile(){ return false;  };

  static std::string constructMessage(const std::vector <std::string>& message);
  virtual std::string constructMessage(unsigned int message);

private:
  std::vector<IGlobalMessageObserver *> m_observers;
};