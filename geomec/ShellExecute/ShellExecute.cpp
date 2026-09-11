// ShellExecute.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cassert>
#include <string>

#include <Windows.h>
#include <Shellapi.h>

int _tmain(int argc, _TCHAR* argv[])
{
  assert(argc > 1);

  size_t length = 0;

  for (int a = 2; a < argc; ++a)
  {
  length += _tcslen(argv[a]);

  if (a != (argc - 1))
  {
      length += _tcslen(TEXT(" "));
  }
  }

  LPTSTR parameters;
  size_t offset = 0;

  parameters = new _TCHAR[length + 1];

  for (int a = 2; a < argc; ++a)
  {
  LPTSTR copied = _tcscpy(&parameters[offset], argv[a]);

  offset += _tcslen(copied);

  if (a != (argc - 1))
  {
      copied = _tcscpy(&parameters[offset], TEXT(" "));
      offset += _tcslen(copied);
  }
  }

  _tprintf(_T("'%s' '%s'"), argv[1], parameters);

  ShellExecute(NULL, TEXT("open"), argv[1], parameters, NULL, SW_HIDE);

  delete [] parameters;

  return 0;
}
