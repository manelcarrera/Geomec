#include "StdAfx.h"
#include <windows.h>

#include <QString>

namespace gm_windows_utils
{


// mostly stolen from the internet
// works like system without the cmd box popping up
QString Execute(const char *command)
{
  QString retval;

  // create pipe
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES) };
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  HANDLE pipeOurEnd, pipeChildEnd;

  if (!CreatePipe(&pipeOurEnd, &pipeChildEnd, &sa, 0))
    return retval;

  STARTUPINFO si = { sizeof(STARTUPINFO) };
  si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;
  si.hStdOutput  = pipeChildEnd;
  si.hStdError   = pipeChildEnd;

  PROCESS_INFORMATION pi = { 0 };

  if (!CreateProcess(NULL, (LPSTR)command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
  {
    CloseHandle(pipeOurEnd);
    CloseHandle(pipeChildEnd);
    return retval;
  }

  char buffer[4096];
  bool bFinished = false;
  
  while (!bFinished)
  {
    bFinished = WaitForSingleObject(pi.hProcess, 100) == WAIT_OBJECT_0;

    while (true)
    {

      DWORD nRead  = 0;
      DWORD nToRead = 0;

      if (!::PeekNamedPipe(pipeOurEnd, NULL, 0, NULL, &nToRead, NULL))
        break;

      if (!nToRead)
        break;


      if (!ReadFile(pipeOurEnd, buffer, min(sizeof(buffer) - 1, nToRead), &nRead, NULL) || !nRead)
        break;

      buffer[nRead] = 0;
      retval += buffer;
    }
  }

  CloseHandle(pipeOurEnd);
  CloseHandle(pipeChildEnd);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return retval;
}



}